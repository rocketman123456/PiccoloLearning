#include "runtime/function/render/interface/vulkan/vulkan_utils.h"
#include "runtime/function/global/global_context.h"
#include "runtime/function/render/interface/vulkan/vulkan_header_includer.h"

#include "runtime/core/base/macro.h"
#include "runtime/core/string/string_utils.h"

#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

#include <glslang/SPIRV/GLSL.ext.EXT.h>
#include <glslang/SPIRV/GLSL.ext.KHR.h>
#include <glslang/SPIRV/GLSL.std.450.h>
#include <glslang/SPIRV/GlslangToSpv.h>
#include <glslang/SPIRV/disassemble.h>
#include <glslang/SPIRV/doc.h>

#include <glslang/Include/ResourceLimits.h>
#include <glslang/Include/ShHandle.h>

#include <glslang/Public/ResourceLimits.h>
#include <glslang/Public/ShaderLang.h>

#include <algorithm>
#include <cmath>
#include <cstring>
#include <exception>
#include <filesystem>
#include <fstream>
#include <stdexcept>

namespace Piccolo
{
    std::unordered_map<uint32_t, VkSampler> VulkanUtils::m_mipmap_sampler_map;
    VkSampler                               VulkanUtils::m_nearest_sampler = VK_NULL_HANDLE;
    VkSampler                               VulkanUtils::m_linear_sampler  = VK_NULL_HANDLE;

    EShLanguage shaderLanguageStageFromFileName(const char* fileName)
    {
        if (end_with(fileName, ".vert"))
            return EShLangVertex;
        if (end_with(fileName, ".frag"))
            return EShLangFragment;
        if (end_with(fileName, ".geom"))
            return EShLangGeometry;
        if (end_with(fileName, ".comp"))
            return EShLangCompute;
        if (end_with(fileName, ".tesc"))
            return EShLangTessControl;
        if (end_with(fileName, ".tese"))
            return EShLangTessEvaluation;
        return EShLangVertex;
    }

    VkShaderModule VulkanUtils::createShaderModule(VkDevice device, const std::string& shader_file)
    {
        std::filesystem::path asset_path       = g_runtime_global_context.m_config_manager->getRootFolder();
        std::filesystem::path shader_file_path = asset_path / "shader" / "glsl" / shader_file;
        std::filesystem::path include_path     = asset_path / "shader" / "include";
        LOG_DEBUG("open shader: " + shader_file_path.generic_string());

        std::string shader_code = "";
        g_runtime_global_context.m_asset_manager->readTextFile(shader_file_path, shader_code);

        EShLanguage stage = shaderLanguageStageFromFileName(shader_file.c_str());

        VulkanHeaderIncluder includer;
        includer.pushExternalLocalDirectory(include_path.generic_string());

        auto client          = glslang::EShClientVulkan;
        auto client_version  = glslang::EShTargetVulkan_1_0;
        auto target_language = glslang::EShTargetSpv;
        auto target_version  = glslang::EShTargetSpv_1_0;
        auto messages        = EShMsgDefault;

        glslang::InitializeProcess();

        glslang::TProgram* program = new glslang::TProgram;
        glslang::TShader*  shader  = new glslang::TShader(stage);

        const char* file_names[]   = {shader_file.c_str()};
        const char* shader_codes[] = {shader_code.c_str()};
        shader->setStringsWithLengthsAndNames(shader_codes, NULL, file_names, 1);

        std::string              preamble_str = "";
        std::vector<std::string> processes    = {};

        shader->setPreamble(preamble_str.c_str());
        shader->addProcesses(processes);

        shader->setEnvInput(glslang::EShSourceGlsl, stage, client, 100);
        shader->setEnvClient(client, client_version);
        shader->setEnvTarget(target_language, target_version);

        const TBuiltInResource* resources      = GetDefaultResources();
        const int               defaultVersion = 100;
        std::string             str;

        if (!shader->preprocess(resources, defaultVersion, ENoProfile, false, false, messages, &str, includer))
        {
            LOG_ERROR(shader->getInfoLog());
            LOG_ERROR(shader->getInfoDebugLog());
            throw std::runtime_error(shader->getInfoLog());
        }

        if (!shader->parse(resources, defaultVersion, false, messages, includer))
        {
            LOG_ERROR(shader->getInfoLog());
            LOG_ERROR(shader->getInfoDebugLog());
            throw std::runtime_error(shader->getInfoLog());
        }

        program->addShader(shader);

        if (!program->link(messages))
        {
            LOG_ERROR(program->getInfoLog());
            LOG_ERROR(program->getInfoDebugLog());
            throw std::runtime_error(program->getInfoLog());
        }

        if (!program->mapIO())
        {
            LOG_ERROR(program->getInfoLog());
            LOG_ERROR(program->getInfoDebugLog());
            throw std::runtime_error(program->getInfoLog());
        }

        bool SpvToolsDisassembler = false;
        bool SpvToolsValidate     = false;

        std::vector<unsigned int> spirv;
        if (program->getIntermediate(stage))
        {
            spv::SpvBuildLogger logger;
            glslang::SpvOptions spvOptions;
            spvOptions.stripDebugInfo   = true;
            spvOptions.disableOptimizer = true;
            spvOptions.optimizeSize     = true;
            spvOptions.disassemble      = SpvToolsDisassembler;
            spvOptions.validate         = SpvToolsValidate;
            glslang::GlslangToSpv(*program->getIntermediate((EShLanguage)stage), spirv, &logger, &spvOptions);
        }
        else
        {
            throw std::runtime_error("cannot find target shader");
        }

        glslang::FinalizeProcess();

        delete shader;
        delete program;

        // copy data to unsigned char;
        std::vector<unsigned char> spirv_char;
        spirv_char.resize(spirv.size() * sizeof(unsigned int));
        memcpy(spirv_char.data(), spirv.data(), spirv_char.size());

        return createShaderModule(device, spirv_char);
    }

    VkShaderModule VulkanUtils::createShaderModule(VkDevice device, const std::vector<unsigned char>& shader_code)
    {
        VkShaderModuleCreateInfo shader_module_create_info {};
        shader_module_create_info.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        shader_module_create_info.codeSize = shader_code.size();
        shader_module_create_info.pCode    = reinterpret_cast<const uint32_t*>(shader_code.data());

        VkShaderModule shader_module;
        if (vkCreateShaderModule(device, &shader_module_create_info, nullptr, &shader_module) != VK_SUCCESS)
        {
            return VK_NULL_HANDLE;
        }
        return shader_module;
    }
} // namespace Piccolo

#include "runtime/function/render/render_system.h"

#include "runtime/core/base/macro.h"

#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

#include "runtime/function/global/global_context.h"
// #include "runtime/function/render/debugdraw/debug_draw_manager.h"
// #include "runtime/function/render/render_camera.h"
// #include "runtime/function/render/render_pass.h"
// #include "runtime/function/render/render_pipeline.h"
// #include "runtime/function/render/render_resource.h"
// #include "runtime/function/render/render_resource_base.h"
// #include "runtime/function/render/render_scene.h"
// #include "runtime/function/render/window_system.h"

// #include "runtime/function/render/passes/main_camera_pass.h"
// #include "runtime/function/render/passes/particle_pass.h"

#include "runtime/function/render/interface/vulkan/vulkan_rhi.h"

namespace Piccolo
{
    void RenderSystem::initialize(RenderSystemInitInfo init_info)
    {
        std::shared_ptr<ConfigManager> config_manager = g_runtime_global_context.m_config_manager;
        ASSERT(config_manager);
        std::shared_ptr<AssetManager> asset_manager = g_runtime_global_context.m_asset_manager;
        ASSERT(asset_manager);

        // render context initialize
        // RHIInitInfo rhi_init_info;
        // rhi_init_info.window_system = init_info.window_system;

        // m_rhi = std::make_shared<VulkanRHI>();
        // m_rhi->initialize(rhi_init_info);
    }
}

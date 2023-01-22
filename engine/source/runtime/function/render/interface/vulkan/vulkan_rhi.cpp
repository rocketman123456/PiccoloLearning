#include "runtime/function/render/interface/vulkan/vulkan_rhi.h"
#include "runtime/function/global/global_context.h"
#include "runtime/function/render/interface/vulkan/vulkan_creation_utils.h"
#include "runtime/function/render/interface/vulkan/vulkan_utils.h"
#include "runtime/function/render/window_system.h"

#include "runtime/core/base/macro.h"

#include "runtime/resource/asset_manager/asset_manager.h"
#include "runtime/resource/config_manager/config_manager.h"

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <limits>
#include <optional>
#include <set>
#include <stdexcept>
#include <vector>

// https://gcc.gnu.org/onlinedocs/cpp/Stringizing.html
#define PICCOLO_XSTR(s) PICCOLO_STR(s)
#define PICCOLO_STR(s) #s

namespace Piccolo
{
    void VulkanRHI::initialize(RHIInitInfo init_info)
    {
        m_window                       = init_info.window_system->getWindow();
        std::array<int, 2> window_size = init_info.window_system->getWindowSize();

        m_viewport = {0.0f, 0.0f, (float)window_size[0], (float)window_size[1], 0.0f, 1.0f};
        m_scissor  = {{0, 0}, {(uint32_t)window_size[0], (uint32_t)window_size[1]}};

        createInstance();
        setupDebugMessenger();
        createSurface();
        pickPhysicalDevice();
        createLogicalDevice();

        createCommandPool();
        createCommandBuffers();

        createSyncPrimitives();

        createSwapChain();
        createSwapChainImageViews();

        // TODO : move to other place
        createRenderPass();
        createGraphicsPipeline();

        createFramebuffers();

        createAssetAllocator();
    }

    void VulkanRHI::prepareContext()
    {
        // TODO
        drawFrame();
    }

    void VulkanRHI::clear()
    {
        vkDeviceWaitIdle(m_device);

        for (size_t i = 0; i < k_max_frames_in_flight; i++)
        {
            vkDestroySemaphore(m_device, m_image_finished_for_presentation_semaphores[i], nullptr);
            vkDestroySemaphore(m_device, m_image_available_for_render_semaphores[i], nullptr);
            vkDestroyFence(m_device, m_is_frame_in_flight_fences[i], nullptr);
        }

        vkDestroyCommandPool(m_device, m_vk_command_pools, nullptr);

        for (auto framebuffer : m_swapchain_framebuffers)
        {
            vkDestroyFramebuffer(m_device, framebuffer, nullptr);
        }

        vkDestroyPipeline(m_device, graphicsPipeline, nullptr);
        vkDestroyPipelineLayout(m_device, pipelineLayout, nullptr);
        vkDestroyRenderPass(m_device, renderPass, nullptr);

        for (auto imageView : m_swapchain_imageviews)
        {
            vkDestroyImageView(m_device, imageView, nullptr);
        }

        vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
        vkDestroyDevice(m_device, nullptr);

        if (m_enable_validation_Layers)
        {
            VulkanCreationUtils::destroyDebugUtilsMessengerEXT(m_instance, m_debug_messenger, nullptr);
        }

        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        vkDestroyInstance(m_instance, nullptr);
    }

    void VulkanRHI::createInstance()
    {
        volkInitialize();

        if (m_enable_validation_Layers && !VulkanCreationUtils::checkValidationLayerSupport(m_validation_layers))
        {
            throw std::runtime_error("validation layers requested, but not available!");
        }

        VkApplicationInfo appInfo {};
        appInfo.sType              = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName   = "piccolo_renderer";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName        = "Piccolo";
        appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion         = k_vulkan_api_version;

        VkInstanceCreateInfo instance_create_info {};
        instance_create_info.sType            = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        instance_create_info.pApplicationInfo = &appInfo;

        auto extensions                              = VulkanCreationUtils::getRequiredExtensions(m_enable_validation_Layers);
        instance_create_info.enabledExtensionCount   = static_cast<uint32_t>(extensions.size());
        instance_create_info.ppEnabledExtensionNames = extensions.data();
#if defined(__MACH__)
        instance_create_info.flags = VK_INSTANCE_CREATE_ENUMERATE_PORTABILITY_BIT_KHR;
#endif

        LOG_DEBUG("Vulkan RHI available extensions: ");
        for (auto extension : extensions)
        {
            LOG_DEBUG("\textension: " + extension);
        }

        VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo {};
        if (m_enable_validation_Layers)
        {
            instance_create_info.enabledLayerCount   = static_cast<uint32_t>(m_validation_layers.size());
            instance_create_info.ppEnabledLayerNames = m_validation_layers.data();

            VulkanCreationUtils::populateDebugMessengerCreateInfo(debugCreateInfo);
            instance_create_info.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
        }
        else
        {
            instance_create_info.enabledLayerCount = 0;
            instance_create_info.pNext             = nullptr;
        }

        if (vkCreateInstance(&instance_create_info, nullptr, &m_instance) != VK_SUCCESS)
        {
            LOG_ERROR("failed to create instance!");
            throw std::runtime_error("failed to create instance!");
        }

        volkLoadInstance(m_instance);
    }

    void VulkanRHI::setupDebugMessenger()
    {
        if (m_enable_validation_Layers)
        {
            VkDebugUtilsMessengerCreateInfoEXT createInfo;
            VulkanCreationUtils::populateDebugMessengerCreateInfo(createInfo);
            if (VulkanCreationUtils::createDebugUtilsMessengerEXT(m_instance, &createInfo, nullptr, &m_debug_messenger) != VK_SUCCESS)
            {
                LOG_ERROR("failed to set up debug messenger!");
                throw std::runtime_error("failed to set up debug messenger!");
            }
        }

        if (m_enable_debug_utils_label)
        {
            // _vkCmdBeginDebugUtilsLabelEXT = (PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetInstanceProcAddr(m_instance, "vkCmdBeginDebugUtilsLabelEXT");
            // _vkCmdEndDebugUtilsLabelEXT   = (PFN_vkCmdEndDebugUtilsLabelEXT)vkGetInstanceProcAddr(m_instance, "vkCmdEndDebugUtilsLabelEXT");
        }
    }

    void VulkanRHI::createSurface()
    {
        if (glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface) != VK_SUCCESS)
        {
            LOG_ERROR("glfwCreateWindowSurface failed!");
            throw std::runtime_error("failed to create window surface!");
        }
    }

    void VulkanRHI::pickPhysicalDevice()
    {
        uint32_t physical_device_count;
        vkEnumeratePhysicalDevices(m_instance, &physical_device_count, nullptr);
        if (physical_device_count == 0)
        {
            LOG_ERROR("enumerate physical devices failed!");
        }
        else
        {
            // find one device that matches our requirement
            // or find which is the best
            std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
            vkEnumeratePhysicalDevices(m_instance, &physical_device_count, physical_devices.data());

            std::vector<std::pair<int, VkPhysicalDevice>> ranked_physical_devices;
            for (const auto& device : physical_devices)
            {
                VkPhysicalDeviceProperties physical_device_properties;
                vkGetPhysicalDeviceProperties(device, &physical_device_properties);
                int score = 0;

                if (physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
                {
                    score += 1000;
                }
                else if (physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
                {
                    score += 100;
                }

                ranked_physical_devices.push_back({score, device});
            }

            std::sort(ranked_physical_devices.begin(),
                      ranked_physical_devices.end(),
                      [](const std::pair<int, VkPhysicalDevice>& p1, const std::pair<int, VkPhysicalDevice>& p2) { return p1 > p2; });

            for (const auto& device : ranked_physical_devices)
            {
                if (VulkanCreationUtils::isDeviceSuitable(device.second, m_surface, m_device_extensions))
                {
                    m_physical_device = device.second;
                    break;
                }
            }

            if (m_physical_device == VK_NULL_HANDLE)
            {
                LOG_ERROR("failed to find suitable physical device");
            }
        }
    }

    void VulkanRHI::createLogicalDevice()
    {
        m_queue_indices = VulkanCreationUtils::findQueueFamilies(m_physical_device, m_surface);

        std::vector<VkDeviceQueueCreateInfo> queue_create_infos; // all queues that need to be created
        std::set<uint32_t>                   queue_families = {
            m_queue_indices.graphics_family.value(), m_queue_indices.present_family.value(), m_queue_indices.compute_family.value()};

        float queue_priority = 1.0f;
        for (uint32_t queue_family : queue_families) // for every queue family
        {
            // queue create info
            VkDeviceQueueCreateInfo queue_create_info {};
            queue_create_info.sType            = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_create_info.queueFamilyIndex = queue_family;
            queue_create_info.queueCount       = 1;
            queue_create_info.pQueuePriorities = &queue_priority;
            queue_create_infos.push_back(queue_create_info);
        }

        // physical device features
        VkPhysicalDeviceFeatures physical_device_features = {};

        physical_device_features.samplerAnisotropy = VK_TRUE;

        // support inefficient readback storage buffer
        physical_device_features.fragmentStoresAndAtomics = VK_TRUE;

        // support independent blending
        physical_device_features.independentBlend = VK_TRUE;

        // support geometry shader
        if (m_enable_point_light_shadow)
        {
            physical_device_features.geometryShader = VK_TRUE;
        }

        // device create info
        VkDeviceCreateInfo device_create_info {};
        device_create_info.sType                   = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        device_create_info.pQueueCreateInfos       = queue_create_infos.data();
        device_create_info.queueCreateInfoCount    = static_cast<uint32_t>(queue_create_infos.size());
        device_create_info.pEnabledFeatures        = &physical_device_features;
        device_create_info.enabledExtensionCount   = static_cast<uint32_t>(m_device_extensions.size());
        device_create_info.ppEnabledExtensionNames = m_device_extensions.data();
        device_create_info.enabledLayerCount       = 0;

        if (vkCreateDevice(m_physical_device, &device_create_info, nullptr, &m_device) != VK_SUCCESS)
        {
            LOG_ERROR("vk create device");
        }

        // initialize queues of this device
        // VkQueue vk_graphics_queue;
        // vkGetDeviceQueue(m_device, m_queue_indices.graphics_family.value(), 0, &vk_graphics_queue);
        // m_graphics_queue = new VulkanQueue();
        // ((VulkanQueue*)m_graphics_queue)->setResource(vk_graphics_queue);

        // VkQueue vk_compute_queue;
        // vkGetDeviceQueue(m_device, m_queue_indices.compute_family.value(), 0, &vk_compute_queue);
        // m_compute_queue = new VulkanQueue();
        // ((VulkanQueue*)m_compute_queue)->setResource(vk_compute_queue);

        vkGetDeviceQueue(m_device, m_queue_indices.graphics_family.value(), 0, &m_graphics_queue);
        vkGetDeviceQueue(m_device, m_queue_indices.compute_family.value(), 0, &m_compute_queue);
        vkGetDeviceQueue(m_device, m_queue_indices.present_family.value(), 0, &m_present_queue);
    }

    void VulkanRHI::createSwapChain()
    {
        // query all supports of this physical device
        SwapChainSupportDetails swapchain_support_details = VulkanCreationUtils::querySwapChainSupport(m_physical_device, m_surface);

        // choose the best or fitting format
        VkSurfaceFormatKHR chosen_surface_format = VulkanCreationUtils::chooseSwapchainSurfaceFormatFromDetails(swapchain_support_details.formats);
        // choose the best or fitting present mode
        VkPresentModeKHR chosen_presentMode = VulkanCreationUtils::chooseSwapchainPresentModeFromDetails(swapchain_support_details.present_modes);
        // choose the best or fitting extent
        VkExtent2D chosen_extent = VulkanCreationUtils::chooseSwapchainExtentFromDetails(swapchain_support_details.capabilities, m_window);

        uint32_t image_count = swapchain_support_details.capabilities.minImageCount + 1;
        if (swapchain_support_details.capabilities.maxImageCount > 0 && image_count > swapchain_support_details.capabilities.maxImageCount)
        {
            image_count = swapchain_support_details.capabilities.maxImageCount;
        }

        VkSwapchainCreateInfoKHR createInfo {};
        createInfo.sType   = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = m_surface;

        createInfo.minImageCount    = image_count;
        createInfo.imageFormat      = chosen_surface_format.format;
        createInfo.imageColorSpace  = chosen_surface_format.colorSpace;
        createInfo.imageExtent      = chosen_extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage       = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        uint32_t queueFamilyIndices[] = {m_queue_indices.graphics_family.value(), m_queue_indices.present_family.value()};

        if (m_queue_indices.graphics_family != m_queue_indices.present_family)
        {
            createInfo.imageSharingMode      = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices   = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform   = swapchain_support_details.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode    = chosen_presentMode;
        createInfo.clipped        = VK_TRUE;

        createInfo.oldSwapchain = VK_NULL_HANDLE;

        if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapchain) != VK_SUCCESS)
        {
            LOG_ERROR("vk create swapchain khr");
        }

        vkGetSwapchainImagesKHR(m_device, m_swapchain, &image_count, nullptr);
        m_swapchain_images.resize(image_count);
        vkGetSwapchainImagesKHR(m_device, m_swapchain, &image_count, m_swapchain_images.data());

        // m_swapchain_image_format  = (RHIFormat)chosen_surface_format.format;
        m_swapchain_image_format  = chosen_surface_format.format;
        m_swapchain_extent.height = chosen_extent.height;
        m_swapchain_extent.width  = chosen_extent.width;

        m_scissor = {{0, 0}, {m_swapchain_extent.width, m_swapchain_extent.height}};
    }

    void VulkanRHI::createSwapChainImageViews()
    {
        m_swapchain_imageviews.resize(m_swapchain_images.size());

        for (size_t i = 0; i < m_swapchain_images.size(); i++)
        {
            VkImageViewCreateInfo createInfo {};
            createInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            createInfo.image                           = m_swapchain_images[i];
            createInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
            createInfo.format                          = m_swapchain_image_format;
            createInfo.components.r                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.g                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.b                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.components.a                    = VK_COMPONENT_SWIZZLE_IDENTITY;
            createInfo.subresourceRange.aspectMask     = VK_IMAGE_ASPECT_COLOR_BIT;
            createInfo.subresourceRange.baseMipLevel   = 0;
            createInfo.subresourceRange.levelCount     = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount     = 1;

            if (vkCreateImageView(m_device, &createInfo, nullptr, &m_swapchain_imageviews[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create image views!");
            }
        }
    }

    void VulkanRHI::createRenderPass()
    {
        VkAttachmentDescription colorAttachment {};
        colorAttachment.format         = m_swapchain_image_format;
        colorAttachment.samples        = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp         = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp        = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp  = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout  = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout    = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

        VkAttachmentReference colorAttachmentRef {};
        colorAttachmentRef.attachment = 0;
        colorAttachmentRef.layout     = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass {};
        subpass.pipelineBindPoint    = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments    = &colorAttachmentRef;

        VkSubpassDependency dependency {};
        dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
        dependency.dstSubpass    = 0;
        dependency.srcStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.srcAccessMask = 0;
        dependency.dstStageMask  = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

        VkRenderPassCreateInfo renderPassInfo {};
        renderPassInfo.sType           = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 1;
        renderPassInfo.pAttachments    = &colorAttachment;
        renderPassInfo.subpassCount    = 1;
        renderPassInfo.pSubpasses      = &subpass;
        renderPassInfo.dependencyCount = 1;
        renderPassInfo.pDependencies   = &dependency;

        if (vkCreateRenderPass(m_device, &renderPassInfo, nullptr, &renderPass) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create render pass!");
        }
    }

    void VulkanRHI::createGraphicsPipeline()
    {
        VkShaderModule vertShaderModule = VulkanUtil::createShaderModule(m_device, "simple_triangle.vert");
        VkShaderModule fragShaderModule = VulkanUtil::createShaderModule(m_device, "simple_triangle.frag");

        VkPipelineShaderStageCreateInfo vertShaderStageInfo {};
        vertShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertShaderStageInfo.stage  = VK_SHADER_STAGE_VERTEX_BIT;
        vertShaderStageInfo.module = vertShaderModule;
        vertShaderStageInfo.pName  = "main";

        VkPipelineShaderStageCreateInfo fragShaderStageInfo {};
        fragShaderStageInfo.sType  = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragShaderStageInfo.stage  = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragShaderStageInfo.module = fragShaderModule;
        fragShaderStageInfo.pName  = "main";

        VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

        VkPipelineVertexInputStateCreateInfo vertexInputInfo {};
        vertexInputInfo.sType                           = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount   = 0;
        vertexInputInfo.vertexAttributeDescriptionCount = 0;

        VkPipelineInputAssemblyStateCreateInfo inputAssembly {};
        inputAssembly.sType                  = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology               = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;

        VkPipelineViewportStateCreateInfo viewportState {};
        viewportState.sType         = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount  = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer {};
        rasterizer.sType                   = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable        = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode             = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth               = 1.0f;
        rasterizer.cullMode                = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace               = VK_FRONT_FACE_CLOCKWISE;
        rasterizer.depthBiasEnable         = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling {};
        multisampling.sType                = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable  = VK_FALSE;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

        VkPipelineColorBlendAttachmentState colorBlendAttachment {};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable    = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending {};
        colorBlending.sType             = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable     = VK_FALSE;
        colorBlending.logicOp           = VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount   = 1;
        colorBlending.pAttachments      = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        std::vector<VkDynamicState>      dynamicStates = {VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR};
        VkPipelineDynamicStateCreateInfo dynamicState {};
        dynamicState.sType             = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates    = dynamicStates.data();

        VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
        pipelineLayoutInfo.sType                  = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount         = 0;
        pipelineLayoutInfo.pushConstantRangeCount = 0;

        if (vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create pipeline layout!");
        }

        VkGraphicsPipelineCreateInfo pipelineInfo {};
        pipelineInfo.sType               = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount          = 2;
        pipelineInfo.pStages             = shaderStages;
        pipelineInfo.pVertexInputState   = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState      = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState   = &multisampling;
        pipelineInfo.pColorBlendState    = &colorBlending;
        pipelineInfo.pDynamicState       = &dynamicState;
        pipelineInfo.layout              = pipelineLayout;
        pipelineInfo.renderPass          = renderPass;
        pipelineInfo.subpass             = 0;
        pipelineInfo.basePipelineHandle  = VK_NULL_HANDLE;

        if (vkCreateGraphicsPipelines(m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &graphicsPipeline) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create graphics pipeline!");
        }

        vkDestroyShaderModule(m_device, fragShaderModule, nullptr);
        vkDestroyShaderModule(m_device, vertShaderModule, nullptr);
    }

    void VulkanRHI::createFramebuffers()
    {
        m_swapchain_framebuffers.resize(m_swapchain_imageviews.size());

        for (size_t i = 0; i < m_swapchain_imageviews.size(); i++)
        {
            VkImageView attachments[] = {m_swapchain_imageviews[i]};

            VkFramebufferCreateInfo framebufferInfo {};
            framebufferInfo.sType           = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
            framebufferInfo.renderPass      = renderPass;
            framebufferInfo.attachmentCount = 1;
            framebufferInfo.pAttachments    = attachments;
            framebufferInfo.width           = m_swapchain_extent.width;
            framebufferInfo.height          = m_swapchain_extent.height;
            framebufferInfo.layers          = 1;

            if (vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &m_swapchain_framebuffers[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create framebuffer!");
            }
        }
    }

    void VulkanRHI::createCommandPool()
    {
        // // default graphics command pool
        // {
        //     m_rhi_command_pool = new VulkanCommandPool();
        //     VkCommandPool           vk_command_pool;
        //     VkCommandPoolCreateInfo command_pool_create_info {};
        //     command_pool_create_info.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        //     command_pool_create_info.pNext            = NULL;
        //     command_pool_create_info.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        //     command_pool_create_info.queueFamilyIndex = m_queue_indices.graphics_family.value();

        //     if (vkCreateCommandPool(m_device, &command_pool_create_info, nullptr, &vk_command_pool) != VK_SUCCESS)
        //     {
        //         LOG_ERROR("vk create command pool");
        //     }

        //     ((VulkanCommandPool*)m_rhi_command_pool)->setResource(vk_command_pool);
        // }

        // // other command pools
        // {
        //     VkCommandPoolCreateInfo command_pool_create_info;
        //     command_pool_create_info.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        //     command_pool_create_info.pNext            = NULL;
        //     command_pool_create_info.flags            = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        //     command_pool_create_info.queueFamilyIndex = m_queue_indices.graphics_family.value();

        //     for (uint32_t i = 0; i < k_max_frames_in_flight; ++i)
        //     {
        //         if (vkCreateCommandPool(m_device, &command_pool_create_info, NULL, &m_vk_command_pools[i]) != VK_SUCCESS)
        //         {
        //             LOG_ERROR("vk create command pool");
        //         }
        //     }
        // }

        QueueFamilyIndices queueFamilyIndices = VulkanCreationUtils::findQueueFamilies(m_physical_device, m_surface);

        VkCommandPoolCreateInfo poolInfo {};
        poolInfo.sType            = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.flags            = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        poolInfo.queueFamilyIndex = queueFamilyIndices.graphics_family.value();

        if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_vk_command_pools) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to create command pool!");
        }
    }

    void VulkanRHI::createCommandBuffers()
    {
        // VkCommandBufferAllocateInfo command_buffer_allocate_info {};
        // command_buffer_allocate_info.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        // command_buffer_allocate_info.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        // command_buffer_allocate_info.commandBufferCount = 1U;

        // for (uint32_t i = 0; i < k_max_frames_in_flight; ++i)
        // {
        //     command_buffer_allocate_info.commandPool = m_vk_command_pools[i];
        //     VkCommandBuffer vk_command_buffer;
        //     if (vkAllocateCommandBuffers(m_device, &command_buffer_allocate_info, &vk_command_buffer) != VK_SUCCESS)
        //     {
        //         LOG_ERROR("vk allocate command buffers");
        //     }
        //     m_vk_command_buffers[i] = vk_command_buffer;
        //     m_command_buffers[i]    = new VulkanCommandBuffer();
        //     ((VulkanCommandBuffer*)m_command_buffers[i])->setResource(vk_command_buffer);
        // }

        m_vk_command_buffers.resize(k_max_frames_in_flight);

        VkCommandBufferAllocateInfo allocInfo {};
        allocInfo.sType              = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool        = m_vk_command_pools;
        allocInfo.level              = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = (uint32_t)m_vk_command_buffers.size();

        if (vkAllocateCommandBuffers(m_device, &allocInfo, m_vk_command_buffers.data()) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate command buffers!");
        }
    }

    void VulkanRHI::recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex)
    {
        VkCommandBufferBeginInfo beginInfo {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo {};
        renderPassInfo.sType             = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass        = renderPass;
        renderPassInfo.framebuffer       = m_swapchain_framebuffers[imageIndex];
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = m_swapchain_extent;

        VkClearValue clearColor        = {{{0.0f, 0.0f, 0.0f, 1.0f}}};
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues    = &clearColor;

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

        VkViewport viewport {};
        viewport.x        = 0.0f;
        viewport.y        = 0.0f;
        viewport.width    = (float)m_swapchain_extent.width;
        viewport.height   = (float)m_swapchain_extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

        VkRect2D scissor {};
        scissor.offset = {0, 0};
        scissor.extent = m_swapchain_extent;
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

        vkCmdDraw(commandBuffer, 3, 1, 0, 0);

        vkCmdEndRenderPass(commandBuffer);

        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to record command buffer!");
        }
    }

    void VulkanRHI::createSyncPrimitives()
    {
        m_image_available_for_render_semaphores.resize(k_max_frames_in_flight);
        m_image_finished_for_presentation_semaphores.resize(k_max_frames_in_flight);
        m_is_frame_in_flight_fences.resize(k_max_frames_in_flight);

        VkSemaphoreCreateInfo semaphoreInfo {};
        semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceInfo {};
        fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        for (size_t i = 0; i < k_max_frames_in_flight; i++)
        {
            if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_image_available_for_render_semaphores[i]) != VK_SUCCESS ||
                vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_image_finished_for_presentation_semaphores[i]) != VK_SUCCESS ||
                vkCreateFence(m_device, &fenceInfo, nullptr, &m_is_frame_in_flight_fences[i]) != VK_SUCCESS)
            {
                throw std::runtime_error("failed to create synchronization objects for a frame!");
            }
        }
    }

    void VulkanRHI::createAssetAllocator()
    {
        VmaVulkanFunctions vma_vulkan_func {};
        vma_vulkan_func.vkAllocateMemory                    = vkAllocateMemory;
        vma_vulkan_func.vkBindBufferMemory                  = vkBindBufferMemory;
        vma_vulkan_func.vkBindImageMemory                   = vkBindImageMemory;
        vma_vulkan_func.vkCreateBuffer                      = vkCreateBuffer;
        vma_vulkan_func.vkCreateImage                       = vkCreateImage;
        vma_vulkan_func.vkDestroyBuffer                     = vkDestroyBuffer;
        vma_vulkan_func.vkDestroyImage                      = vkDestroyImage;
        vma_vulkan_func.vkFlushMappedMemoryRanges           = vkFlushMappedMemoryRanges;
        vma_vulkan_func.vkFreeMemory                        = vkFreeMemory;
        vma_vulkan_func.vkGetBufferMemoryRequirements       = vkGetBufferMemoryRequirements;
        vma_vulkan_func.vkGetImageMemoryRequirements        = vkGetImageMemoryRequirements;
        vma_vulkan_func.vkGetPhysicalDeviceMemoryProperties = vkGetPhysicalDeviceMemoryProperties;
        vma_vulkan_func.vkGetPhysicalDeviceProperties       = vkGetPhysicalDeviceProperties;
        vma_vulkan_func.vkInvalidateMappedMemoryRanges      = vkInvalidateMappedMemoryRanges;
        vma_vulkan_func.vkMapMemory                         = vkMapMemory;
        vma_vulkan_func.vkUnmapMemory                       = vkUnmapMemory;
        vma_vulkan_func.vkCmdCopyBuffer                     = vkCmdCopyBuffer;

        VmaAllocatorCreateInfo allocatorCreateInfo = {};
        allocatorCreateInfo.vulkanApiVersion       = k_vulkan_api_version;
        allocatorCreateInfo.physicalDevice         = m_physical_device;
        allocatorCreateInfo.device                 = m_device;
        allocatorCreateInfo.instance               = m_instance;
        allocatorCreateInfo.pVulkanFunctions       = &vma_vulkan_func;

        vmaCreateAllocator(&allocatorCreateInfo, &m_assets_allocator);
    }

    void VulkanRHI::drawFrame()
    {
        vkWaitForFences(m_device, 1, &m_is_frame_in_flight_fences[m_current_frame_index], VK_TRUE, UINT64_MAX);
        vkResetFences(m_device, 1, &m_is_frame_in_flight_fences[m_current_frame_index]);

        uint32_t imageIndex;
        vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, m_image_available_for_render_semaphores[m_current_frame_index], VK_NULL_HANDLE, &imageIndex);

        vkResetCommandBuffer(m_vk_command_buffers[m_current_frame_index], /*VkCommandBufferResetFlagBits*/ 0);
        recordCommandBuffer(m_vk_command_buffers[m_current_frame_index], imageIndex);

        VkSubmitInfo submitInfo {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

        VkSemaphore          waitSemaphores[] = {m_image_available_for_render_semaphores[m_current_frame_index]};
        VkPipelineStageFlags waitStages[]     = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
        submitInfo.waitSemaphoreCount         = 1;
        submitInfo.pWaitSemaphores            = waitSemaphores;
        submitInfo.pWaitDstStageMask          = waitStages;

        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers    = &m_vk_command_buffers[m_current_frame_index];

        VkSemaphore signalSemaphores[]  = {m_image_finished_for_presentation_semaphores[m_current_frame_index]};
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores    = signalSemaphores;

        if (vkQueueSubmit(m_graphics_queue, 1, &submitInfo, m_is_frame_in_flight_fences[m_current_frame_index]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to submit draw command buffer!");
        }

        VkPresentInfoKHR presentInfo {};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores    = signalSemaphores;

        VkSwapchainKHR swapChains[] = {m_swapchain};
        presentInfo.swapchainCount  = 1;
        presentInfo.pSwapchains     = swapChains;

        presentInfo.pImageIndices = &imageIndex;

        vkQueuePresentKHR(m_present_queue, &presentInfo);

        m_current_frame_index = (m_current_frame_index + 1) % k_max_frames_in_flight;
    }

} // namespace Piccolo

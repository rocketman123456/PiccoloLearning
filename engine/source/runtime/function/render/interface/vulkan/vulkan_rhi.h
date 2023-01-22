#pragma once
#include "runtime/function/render/interface/vulkan/vulkan_rhi_resource.h"

#include <volk.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace Piccolo
{
    class VulkanRHI final : public RHI
    {
    public:
        virtual ~VulkanRHI() = default;

        void initialize(RHIInitInfo init_info) override;
        void prepareContext() override;

        void clear() override;

        void drawFrame();

    private:
        void createInstance();
        void setupDebugMessenger();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();
        void createSwapChain();
        void createSwapChainImageViews();
        void createFramebuffers();
        void createCommandPool();
        void createCommandBuffers();
        void createSyncPrimitives();
        void createAssetAllocator();

        void createRenderPass();
        void createGraphicsPipeline();

        void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

    private:
        const std::vector<const char*> m_validation_layers = {"VK_LAYER_KHRONOS_validation"};
        const std::vector<const char*> m_device_extensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
#if defined(__MACH__)
            "VK_KHR_portability_subset",
#endif
        };

        const uint8_t  k_max_frames_in_flight {3};
        const uint32_t k_vulkan_api_version {VK_API_VERSION_1_0};

        bool m_enable_validation_Layers {true};
        bool m_enable_debug_utils_label {true};
        bool m_enable_point_light_shadow {true};

        // used in descriptor pool creation
        uint32_t m_max_vertex_blending_mesh_count {256};
        uint32_t m_max_material_count {256};

        RHIViewport m_viewport;
        RHIRect2D   m_scissor;

        GLFWwindow*      m_window {nullptr};
        VkInstance       m_instance {nullptr};
        VkSurfaceKHR     m_surface {nullptr};
        VkPhysicalDevice m_physical_device {nullptr};
        VkDevice         m_device {nullptr};

        VkDebugUtilsMessengerEXT m_debug_messenger;

        QueueFamilyIndices m_queue_indices;

        VkQueue m_graphics_queue;
        VkQueue m_compute_queue;
        VkQueue m_present_queue;

        VkSwapchainKHR           m_swapchain;
        std::vector<VkImage>     m_swapchain_images;
        VkFormat                 m_swapchain_image_format;
        std::vector<VkImageView> m_swapchain_imageviews;
        VkExtent2D               m_swapchain_extent;

        std::vector<VkFramebuffer> m_swapchain_framebuffers;

        // RHIFormat                  m_swapchain_image_format {RHI_FORMAT_UNDEFINED};
        // std::vector<RHIImageView*> m_swapchain_imageviews;
        // RHIExtent2D                m_swapchain_extent;

        VmaAllocator m_assets_allocator;

        VkCommandPool                m_vk_command_pools;
        std::vector<VkCommandBuffer> m_vk_command_buffers;

        uint8_t                  m_current_frame_index = 0;
        std::vector<VkSemaphore> m_image_available_for_render_semaphores;
        std::vector<VkSemaphore> m_image_finished_for_presentation_semaphores;
        std::vector<VkFence>     m_is_frame_in_flight_fences;

        // TODO : move to other place
        VkRenderPass     renderPass;
        VkPipelineLayout pipelineLayout;
        VkPipeline       graphicsPipeline;
    };
} // namespace Piccolo

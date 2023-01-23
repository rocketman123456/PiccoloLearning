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
        void clearSwapchain() override;
        void destroyDefaultSampler(RHIDefaultSamplerType type) override;
        void destroyMipmappedSampler() override;
        void destroyShaderModule(RHIShader* shader) override;
        void destroySemaphore(RHISemaphore* semaphore) override;
        void destroySampler(RHISampler* sampler) override;
        void destroyInstance(RHIInstance* instance) override;
        void destroyImageView(RHIImageView* imageView) override;
        void destroyImage(RHIImage* image) override;
        void destroyFramebuffer(RHIFramebuffer* framebuffer) override;
        void destroyFence(RHIFence* fence) override;
        void destroyDevice(RHIDevice* device) override;
        void destroyCommandPool(RHICommandPool* commandPool) override;
        void destroyBuffer(RHIBuffer*& buffer) override;
        void freeCommandBuffers(RHICommandPool* commandPool, uint32_t commandBufferCount, RHICommandBuffer* pCommandBuffers) override;

        // ------------------------------------------------------------------
        // ------------------------------------------------------------------
        // ------------------------------------------------------------------

        void createInstance();
        void setupDebugMessenger();
        void createSurface();
        void pickPhysicalDevice();
        void createLogicalDevice();

        void createSwapchain() override;
        void recreateSwapchain() override;
        void createSwapchainImageViews() override;
        void createFramebufferImageAndView() override;

        // TODO
        void createRenderPass();
        void createGraphicsPipeline();

        void createCommandPool();
        void createCommandBuffers();
        void createSyncPrimitives();
        void createAssetAllocator();

        void recordCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex);

        void drawFrame();

    public:
        const std::vector<const char*> m_validation_layers = {"VK_LAYER_KHRONOS_validation"};
        const std::vector<const char*> m_device_extensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
#if defined(__MACH__)
            "VK_KHR_portability_subset",
#endif
        };

        static const uint8_t  k_max_frames_in_flight {3};
        static const uint32_t k_vulkan_api_version {VK_API_VERSION_1_0};

        bool m_enable_validation_Layers {true};
        bool m_enable_debug_utils_label {true};
        bool m_enable_point_light_shadow {true};

        // used in descriptor pool creation
        uint32_t m_max_vertex_blending_mesh_count {256};
        uint32_t m_max_material_count {256};

        GLFWwindow* m_window {nullptr};

        std::shared_ptr<RHIInstance>       m_instance;
        std::shared_ptr<RHISurface>        m_surface;
        std::shared_ptr<RHIPhysicalDevice> m_physical_device;
        std::shared_ptr<RHIDevice>         m_device;

        VkInstance       m_vk_instance {nullptr};
        VkSurfaceKHR     m_vk_surface {nullptr};
        VkPhysicalDevice m_vk_physical_device {nullptr};
        VkDevice         m_vk_device {nullptr};

        VkDebugUtilsMessengerEXT m_debug_messenger;

        QueueFamilyIndices m_queue_indices;

        std::shared_ptr<RHIQueue> m_graphics_queue {nullptr};
        std::shared_ptr<RHIQueue> m_compute_queue {nullptr};
        std::shared_ptr<RHIQueue> m_present_queue {nullptr};

        VkQueue m_vk_graphics_queue;
        VkQueue m_vk_compute_queue;
        VkQueue m_vk_present_queue;

        VkSwapchainKHR             m_swapchain;
        std::vector<VkImage>       m_swapchain_images;
        std::vector<VkFramebuffer> m_swapchain_framebuffers;

        RHIFormat                  m_swapchain_image_format {RHI_FORMAT_UNDEFINED};
        std::vector<RHIImageView*> m_swapchain_imageviews;
        RHIExtent2D                m_swapchain_extent;
        RHIViewport                m_viewport;
        RHIRect2D                  m_scissor;

        VmaAllocator m_assets_allocator;

        // command pool and buffers
        std::shared_ptr<RHICommandPool>   m_rhi_command_pool;
        std::shared_ptr<RHICommandBuffer> m_command_buffers[k_max_frames_in_flight];
        std::shared_ptr<RHICommandBuffer> m_current_command_buffer = std::make_shared<VulkanCommandBuffer>();

        VkCommandPool   m_vk_rhi_command_pool;
        VkCommandBuffer m_vk_rhi_command_buffers[k_max_frames_in_flight];

        VkCommandPool   m_vk_command_pools[k_max_frames_in_flight];
        VkCommandBuffer m_vk_command_buffers[k_max_frames_in_flight];

        // sync objects
        uint8_t                  m_current_frame_index {0};
        std::vector<VkSemaphore> m_image_available_for_render_semaphores;
        std::vector<VkSemaphore> m_image_finished_for_presentation_semaphores;
        std::vector<VkSemaphore> m_image_available_for_texturescopy_semaphores;
        std::vector<VkFence>     m_is_frame_in_flight_fences;

        // TODO
        VkRenderPass     renderPass;
        VkPipelineLayout pipelineLayout;
        VkPipeline       graphicsPipeline;
    };
} // namespace Piccolo

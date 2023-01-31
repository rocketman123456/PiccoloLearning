#pragma once
#include "runtime/function/render/interface/vulkan/vulkan_rhi_resource.h"
#include "runtime/function/render/interface/vulkan/vulkan_tools.h"

#include <volk.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <vector>

#ifdef __APPLE__
#include <sys/utsname.h>
#endif

namespace Piccolo
{
    class VulkanSwapchain
    {
    private:
        VkInstance       instance;
        VkDevice         device;
        VkPhysicalDevice physicalDevice;
        VkSurfaceKHR     surface;

    public:
        VkFormat                     colorFormat;
        VkColorSpaceKHR              colorSpace;
        VkSwapchainKHR               swapChain = VK_NULL_HANDLE;
        uint32_t                     imageCount;
        std::vector<VkImage>         images;
        std::vector<SwapChainBuffer> buffers;
        uint32_t                     queueNodeIndex = UINT32_MAX;

        void     initSurface(GLFWwindow* window);
        void     connect(VkInstance instance, VkPhysicalDevice physicalDevice, VkDevice device);
        void     create(uint32_t* width, uint32_t* height, bool vsync = false, bool fullscreen = false);
        VkResult acquireNextImage(VkSemaphore presentCompleteSemaphore, uint32_t* imageIndex);
        VkResult queuePresent(VkQueue queue, uint32_t imageIndex, VkSemaphore waitSemaphore = VK_NULL_HANDLE);
        void     cleanup();
    };
} // namespace Piccolo

#include "runtime/function/render/interface/vulkan/vulkan_swapchain.h"

namespace Piccolo
{
    void VulkanSwapchain::initSurface(GLFWwindow* window)
    {
        auto result = glfwCreateWindowSurface(instance, window, nullptr, &surface);
        if (result != VK_SUCCESS)
        {
            VulkanTools::exitFatal("Could not create surface!", result);
        }

        // Get available queue family properties
        uint32_t queueCount;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, NULL);
        assert(queueCount >= 1);

        std::vector<VkQueueFamilyProperties> queueProps(queueCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, queueProps.data());

        // Iterate over each queue to learn whether it supports presenting:
        // Find a queue with present support
        // Will be used to present the swap chain images to the windowing system
        std::vector<VkBool32> supportsPresent(queueCount);
        for (uint32_t i = 0; i < queueCount; i++)
        {
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, surface, &supportsPresent[i]);
        }

        // Search for a graphics and a present queue in the array of queue
        // families, try to find one that supports both
        uint32_t graphicsQueueNodeIndex = UINT32_MAX;
        uint32_t presentQueueNodeIndex  = UINT32_MAX;
        for (uint32_t i = 0; i < queueCount; i++)
        {
            if ((queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
            {
                if (graphicsQueueNodeIndex == UINT32_MAX)
                {
                    graphicsQueueNodeIndex = i;
                }

                if (supportsPresent[i] == VK_TRUE)
                {
                    graphicsQueueNodeIndex = i;
                    presentQueueNodeIndex  = i;
                    break;
                }
            }
        }
        if (presentQueueNodeIndex == UINT32_MAX)
        {
            // If there's no queue that supports both present and graphics
            // try to find a separate present queue
            for (uint32_t i = 0; i < queueCount; ++i)
            {
                if (supportsPresent[i] == VK_TRUE)
                {
                    presentQueueNodeIndex = i;
                    break;
                }
            }
        }

        // Exit if either a graphics or a presenting queue hasn't been found
        if (graphicsQueueNodeIndex == UINT32_MAX || presentQueueNodeIndex == UINT32_MAX)
        {
            VulkanTools::exitFatal("Could not find a graphics and/or presenting queue!", -1);
        }

        // todo : Add support for separate graphics and presenting queue
        if (graphicsQueueNodeIndex != presentQueueNodeIndex)
        {
            VulkanTools::exitFatal("Separate graphics and presenting queues are not supported yet!", -1);
        }

        queueNodeIndex = graphicsQueueNodeIndex;

        // Get list of supported surface formats
        uint32_t formatCount;
        VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, NULL));
        assert(formatCount > 0);

        std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);
        VK_CHECK_RESULT(vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, surfaceFormats.data()));

        // If the surface format list only includes one entry with VK_FORMAT_UNDEFINED,
        // there is no preferred format, so we assume VK_FORMAT_B8G8R8A8_UNORM
        if ((formatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED))
        {
            colorFormat = VK_FORMAT_B8G8R8A8_UNORM;
            colorSpace  = surfaceFormats[0].colorSpace;
        }
        else
        {
            // iterate over the list of available surface format and
            // check for the presence of VK_FORMAT_B8G8R8A8_UNORM
            bool found_B8G8R8A8_UNORM = false;
            for (auto&& surfaceFormat : surfaceFormats)
            {
                if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM)
                {
                    colorFormat          = surfaceFormat.format;
                    colorSpace           = surfaceFormat.colorSpace;
                    found_B8G8R8A8_UNORM = true;
                    break;
                }
            }

            // in case VK_FORMAT_B8G8R8A8_UNORM is not available
            // select the first available color format
            if (!found_B8G8R8A8_UNORM)
            {
                colorFormat = surfaceFormats[0].format;
                colorSpace  = surfaceFormats[0].colorSpace;
            }
        }
    }

} // namespace Piccolo

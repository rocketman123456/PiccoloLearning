#pragma once
#include "runtime/function/render/interface/vulkan/vulkan_rhi_resource.h"

#include <volk.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <vector>

namespace Piccolo
{
    class VulkanCreationUtils
    {
    public:
        static VkResult createDebugUtilsMessengerEXT(VkInstance                                instance,
                                                     const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
                                                     const VkAllocationCallbacks*              pAllocator,
                                                     VkDebugUtilsMessengerEXT*                 pDebugMessenger);

        static void destroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);

        static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

        static VkSurfaceFormatKHR chooseSwapchainSurfaceFormatFromDetails(const std::vector<VkSurfaceFormatKHR>& availableFormats);

        static VkPresentModeKHR chooseSwapchainPresentModeFromDetails(const std::vector<VkPresentModeKHR>& availablePresentModes);

        static VkExtent2D chooseSwapchainExtentFromDetails(const VkSurfaceCapabilitiesKHR& capabilities, GLFWwindow* window);

        static SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR surface);

        static bool isDeviceSuitable(VkPhysicalDevice device, VkSurfaceKHR surface, const std::vector<const char*>& requiredExtension);

        static bool checkDeviceExtensionSupport(VkPhysicalDevice device, const std::vector<const char*>& requiredExtension);

        static QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device, VkSurfaceKHR surface);

        static std::vector<const char*> getRequiredExtensions(bool enable_validation_layer);

        static bool checkValidationLayerSupport(const std::vector<const char*>& validationLayers);
    };

} // namespace Piccolo

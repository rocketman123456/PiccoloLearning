#pragma once
#include "runtime/core/base/macro.h"

#include <volk.h>

#include <iostream>
#include <string>

// Custom define for better code readability
#define VK_FLAGS_NONE 0
// Default fence timeout in nanoseconds
#define DEFAULT_FENCE_TIMEOUT 100000000000

#if (defined(VK_USE_PLATFORM_IOS_MVK) || defined(VK_USE_PLATFORM_MACOS_MVK))
// SRS - Enable beta extensions and make VK_KHR_portability_subset visible
#define VK_ENABLE_BETA_EXTENSIONS
#endif

#define VK_CHECK_RESULT(f) \
    { \
        VkResult res = (f); \
        if (res != VK_SUCCESS) \
        { \
            LOG_ERROR("Fatal : VkResult is \"{}\" in {}  at line ", VulkanTools::errorString(res), __FILE__, __LINE__); \
            assert(res == VK_SUCCESS); \
        } \
    }

namespace Piccolo
{
    class VulkanTools
    {
    public:
        /** @brief Disable message boxes on fatal errors */
        static bool errorModeSilent;

        /** @brief Returns an error code as a string */
        static std::string errorString(VkResult errorCode);

        /** @brief Returns the device type as a string */
        static std::string physicalDeviceTypeString(VkPhysicalDeviceType type);

        // Selected a suitable supported depth format starting with 32 bit down to 16 bit
        // Returns false if none of the depth formats in the list is supported by the device
        static VkBool32 getSupportedDepthFormat(VkPhysicalDevice physicalDevice, VkFormat* depthFormat);

        // Returns tru a given format support LINEAR filtering
        static VkBool32 formatIsFilterable(VkPhysicalDevice physicalDevice, VkFormat format, VkImageTiling tiling);
        // Returns true if a given format has a stencil part
        static VkBool32 formatHasStencil(VkFormat format);

        // Put an image memory barrier for setting an image layout on the sub resource into the given command buffer
        static void setImageLayout(VkCommandBuffer         cmdbuffer,
                                   VkImage                 image,
                                   VkImageLayout           oldImageLayout,
                                   VkImageLayout           newImageLayout,
                                   VkImageSubresourceRange subresourceRange,
                                   VkPipelineStageFlags    srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                                   VkPipelineStageFlags    dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
        // Uses a fixed sub resource layout with first mip level and layer
        static void setImageLayout(VkCommandBuffer      cmdbuffer,
                                   VkImage              image,
                                   VkImageAspectFlags   aspectMask,
                                   VkImageLayout        oldImageLayout,
                                   VkImageLayout        newImageLayout,
                                   VkPipelineStageFlags srcStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                                   VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

        /** @brief Insert an image memory barrier into the command buffer */
        static void insertImageMemoryBarrier(VkCommandBuffer         cmdbuffer,
                                             VkImage                 image,
                                             VkAccessFlags           srcAccessMask,
                                             VkAccessFlags           dstAccessMask,
                                             VkImageLayout           oldImageLayout,
                                             VkImageLayout           newImageLayout,
                                             VkPipelineStageFlags    srcStageMask,
                                             VkPipelineStageFlags    dstStageMask,
                                             VkImageSubresourceRange subresourceRange);

        // Display error message and exit on fatal error
        static void exitFatal(const std::string& message, int32_t exitCode);
        static void exitFatal(const std::string& message, VkResult resultCode);

        static uint32_t alignedSize(uint32_t value, uint32_t alignment);
    };
} // namespace Piccolo

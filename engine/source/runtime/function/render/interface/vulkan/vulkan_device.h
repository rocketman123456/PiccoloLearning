#pragma once
#include <volk.h>

#include <string>
#include <vector>

namespace Piccolo
{
    class VulkanDevice
    {
    public:
        // @brief Physical device representation
        VkPhysicalDevice m_physical_device;
        // @brief Logical device representation (application's view of the device)
        VkDevice m_logical_device;
        // @brief Properties of the physical device including limits that the application can check against
        VkPhysicalDeviceProperties m_properties;
        // @brief Features of the physical device that an application can use to check if a feature is supported
        VkPhysicalDeviceFeatures m_features;
        // @brief Features that have been enabled for use on the physical device
        VkPhysicalDeviceFeatures m_enabled_features;
        // @brief Memory types and heaps of the physical device
        VkPhysicalDeviceMemoryProperties m_memory_properties;
        // @brief Queue family properties of the physical device
        std::vector<VkQueueFamilyProperties> m_queue_family_properties;
        // @brief List of extensions supported by the device
        std::vector<std::string> m_supported_extensions;
        // @brief Default command pool for the graphics queue family index
        VkCommandPool m_command_pool = VK_NULL_HANDLE;
        // @brief Set to true when the debug marker extension is detected
        bool m_enable_debug_markers = false;
        // @brief Contains queue family indices
        struct
        {
            uint32_t graphics;
            uint32_t compute;
            uint32_t transfer;
        } m_queue_family_indices;

        operator VkDevice() const { return m_logical_device; };

        explicit VulkanDevice(VkPhysicalDevice physicalDevice);
        ~VulkanDevice();

        uint32_t getMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties, VkBool32* memTypeFound = nullptr) const;
        uint32_t getQueueFamilyIndex(VkQueueFlags queueFlags) const;

        VkResult createLogicalDevice(VkPhysicalDeviceFeatures enabledFeatures,
                                     std::vector<const char*> enabledExtensions,
                                     void*                    pNextChain,
                                     bool                     useSwapChain        = true,
                                     VkQueueFlags             requestedQueueTypes = VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);

        VkResult createBuffer(VkBufferUsageFlags    usageFlags,
                              VkMemoryPropertyFlags memoryPropertyFlags,
                              VkDeviceSize          size,
                              VkBuffer*             buffer,
                              VkDeviceMemory*       memory,
                              void*                 data = nullptr);

        //VkResult        createBuffer(VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, vks::Buffer* buffer, VkDeviceSize size, void* data = nullptr);
        //void            copyBuffer(vks::Buffer* src, vks::Buffer* dst, VkQueue queue, VkBufferCopy* copyRegion = nullptr);
        VkCommandPool   createCommandPool(uint32_t queueFamilyIndex, VkCommandPoolCreateFlags createFlags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);
        VkCommandBuffer createCommandBuffer(VkCommandBufferLevel level, VkCommandPool pool, bool begin = false);
        VkCommandBuffer createCommandBuffer(VkCommandBufferLevel level, bool begin = false);
        void            flushCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue, VkCommandPool pool, bool free = true);
        void            flushCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue, bool free = true);
        bool            extensionSupported(std::string extension);
        VkFormat        getSupportedDepthFormat(bool checkSamplingSupport);
    };
} // namespace Piccolo

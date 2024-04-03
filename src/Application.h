#pragma once

#include <vulkan/vulkan.h>

#include "2023/DayOne.h"
#include "2023/DayTwo.h"

#include <iostream>
#include <vector>

struct QueueFamilyIndices
{
    uint32_t ComputeFamily = (uint32_t)-1;

    inline bool IsComplete() const { return ComputeFamily != (uint32_t)-1; }
};

namespace Utils {

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger);
    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator);
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    std::vector<char> ReadFile(const std::string& filename);
    uint32_t FindMemoryType(VkPhysicalDevice device, uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void CreateBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    VkCommandBuffer BeginSingleTimeCommands();
    void EndSingleTimeCommands(VkCommandBuffer commandBuffer);
    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

}

class Application
{
public:
    Application();
    ~Application();
private:
    void SetupInstanceAndDevice();
    void SetupDescriptorAndCommandPools();
    void SetupComputePipeline();

    bool CheckValidationLayerSupport();
    bool IsDeviceSuitable(VkPhysicalDevice device);
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
    VkShaderModule CreateShaderModule(const std::vector<char>& code);
private:
    VkInstance m_Instance = nullptr;
    VkDebugUtilsMessengerEXT m_DebugMessenger = nullptr;
    VkPhysicalDevice m_PhysicalDevice = nullptr;
    VkDevice m_Device = nullptr;
    VkQueue m_ComputeQueue = nullptr;
    VkPipelineLayout m_PipelineLayout = nullptr;
    VkDescriptorPool m_DescriptorPool = nullptr;
    VkCommandPool m_CommandPool = nullptr;
    VkDescriptorSetLayout m_DescriptorSetLayout = nullptr;

    VkBuffer m_InputBuffer = nullptr;
    VkDeviceMemory m_InputBufferMemory = nullptr;
    VkBuffer m_OutputBuffer = nullptr;
    VkDeviceMemory m_OutputBufferMemory = nullptr;

    DayOne* m_DayOne = nullptr;
    DayTwo* m_DayTwo = nullptr;

    friend VkCommandBuffer Utils::BeginSingleTimeCommands();
    friend void Utils::EndSingleTimeCommands(VkCommandBuffer commandBuffer);
    friend class DayOne;
    friend class DayTwo;
};

#define ASSERT(x) { if (!(x)) { std::cerr << "\u001b[31mAssertion failed: " << #x << "\u001b[0m" << std::endl; } }
#define VK_CHECK(result, msg) ASSERT(result == VK_SUCCESS && msg)

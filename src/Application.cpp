#include "Application.h"

#include "Timer.h"

#include <vector>
#include <array>
#include <fstream>

const std::vector<const char*> g_ValidationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

#ifdef _DEBUG
    constexpr bool g_EnableValidationLayers = true;
#else
    constexpr bool g_EnableValidationLayers = false;
#endif

static Application* s_Instance = nullptr;

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData)
{
    if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
        std::cerr << "\u001b[31mValidation layer: " << pCallbackData->pMessage << "\u001b[0m" << std::endl;
    else if (messageSeverity >= VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
        std::cerr << "\033[38;5;208mValidation layer: " << pCallbackData->pMessage << "\u001b[0m" << std::endl;

    return VK_FALSE;
}

namespace Utils {

    VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
    {
        auto func = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
        }
        else
        {
            return VK_ERROR_EXTENSION_NOT_PRESENT;
        }
    }

    void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator)
    {
        auto func = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
        if (func != nullptr)
        {
            func(instance, debugMessenger, pAllocator);
        }
    }

    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo)
    {
        createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        createInfo.pfnUserCallback = DebugCallback;
    }

    std::vector<char> ReadFile(const std::string& filename)
    {
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (!file.is_open())
        {
            std::cerr << "Error opening file: " << filename << std::endl;
            return std::vector<char>(); // return an empty vector on failure
        }

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg); // Move file cursor back to the beginning

        std::vector<char> buffer(size);
        if (file.read(buffer.data(), size))
        {
            return buffer;
        }
        else
        {
            std::cerr << "Error reading file: " << filename << std::endl;
            return std::vector<char>(); // return an empty vector on failure
        }
    }

    uint32_t FindMemoryType(VkPhysicalDevice device, uint32_t typeFilter, VkMemoryPropertyFlags properties)
    {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(device, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
        {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
            {
                return i;
            }
        }

        ASSERT(false && "Failed to find suitable memory type!");
        return 0;
    }

    void CreateBuffer(VkDevice device, VkPhysicalDevice physicalDevice, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VkResult result = vkCreateBuffer(device, &bufferInfo, nullptr, &buffer);
        VK_CHECK(result, "Failed to create Vulkan buffer!");

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = FindMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

        result = vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory);
        VK_CHECK(result, "Failed to allocate Vulkan memory!");

        vkBindBufferMemory(device, buffer, bufferMemory, 0);
    }

    VkCommandBuffer BeginSingleTimeCommands()
    {
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = s_Instance->m_CommandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer commandBuffer;
        vkAllocateCommandBuffers(s_Instance->m_Device, &allocInfo, &commandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(commandBuffer, &beginInfo);

        return commandBuffer;
    }

    void EndSingleTimeCommands(VkCommandBuffer commandBuffer)
    {
        vkEndCommandBuffer(commandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;

        vkQueueSubmit(s_Instance->m_ComputeQueue, 1, &submitInfo, nullptr);
        vkQueueWaitIdle(s_Instance->m_ComputeQueue);

        vkFreeCommandBuffers(s_Instance->m_Device, s_Instance->m_CommandPool, 1, &commandBuffer);
    }

    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) 
    {
        VkCommandBuffer commandBuffer = BeginSingleTimeCommands();

        VkBufferCopy copyRegion{};
        copyRegion.size = size;
        vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

        EndSingleTimeCommands(commandBuffer);
    }

}

Application::Application()
{
    s_Instance = this;

    SetupInstanceAndDevice();
    SetupDescriptorAndCommandPools();
    SetupComputePipeline();

    m_DayOne = new DayOne(this);
    ExecutionResult result = m_DayOne->Run();

    std::cout << "----- Day 1 -----" << std::endl;
    std::cout << "Result (part 1): " << result.PartOne.IntResult << std::endl;
    std::cout << "Result (part 2): " << result.PartTwo.IntResult << std::endl;
    std::cout << "Elapsed time: " << result.TimeMillis << "ms" << std::endl;
}

Application::~Application()
{
    vkDeviceWaitIdle(m_Device);

    delete m_DayOne;

    vkDestroyCommandPool(m_Device, m_CommandPool, nullptr);
    vkDestroyDescriptorSetLayout(m_Device, m_DescriptorSetLayout, nullptr);
    vkDestroyDescriptorPool(m_Device, m_DescriptorPool, nullptr);

    vkDestroyPipelineLayout(m_Device, m_PipelineLayout, nullptr);

    vkDestroyDevice(m_Device, nullptr);

    if (g_EnableValidationLayers)
    {
        Utils::DestroyDebugUtilsMessengerEXT(m_Instance, m_DebugMessenger, nullptr);
    }

    vkDestroyInstance(m_Instance, nullptr);
}

void Application::SetupInstanceAndDevice()
{
    if (g_EnableValidationLayers)
    {
        ASSERT(CheckValidationLayerSupport());
    }

    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Advent of Code";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "No Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_2;

    std::vector<const char*> extensions;
    if (g_EnableValidationLayers)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    VkValidationFeatureEnableEXT enables[] = { VK_VALIDATION_FEATURE_ENABLE_DEBUG_PRINTF_EXT };
    VkValidationFeaturesEXT features = {};
    features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
    features.enabledValidationFeatureCount = 1;
    features.pEnabledValidationFeatures = enables;

    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = (uint32_t)extensions.size();
    createInfo.ppEnabledExtensionNames = extensions.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
    if (g_EnableValidationLayers)
    {
        createInfo.enabledLayerCount = static_cast<uint32_t>(g_ValidationLayers.size());
        createInfo.ppEnabledLayerNames = g_ValidationLayers.data();

        Utils::PopulateDebugMessengerCreateInfo(debugCreateInfo);
        debugCreateInfo.pNext = &features;
        createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
    }
    else
    {
        createInfo.enabledLayerCount = 0;
        createInfo.pNext = nullptr;
    }

    VkResult result = vkCreateInstance(&createInfo, nullptr, &m_Instance);
    VK_CHECK(result, "Failed to create Vulkan instance!");

    if (g_EnableValidationLayers)
    {
        VkDebugUtilsMessengerCreateInfoEXT createInfo;
        Utils::PopulateDebugMessengerCreateInfo(createInfo);

        result = Utils::CreateDebugUtilsMessengerEXT(m_Instance, &createInfo, nullptr, &m_DebugMessenger);
        VK_CHECK(result, "Failed to create Vulkan debug messenger!");
    }

    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_Instance, &deviceCount, nullptr);

    ASSERT(deviceCount && "Failed to find GPUs with Vulkan support!");

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

    for (const auto& device : devices)
    {
        if (IsDeviceSuitable(device))
        {
            m_PhysicalDevice = device;
            break;
        }
    }

    ASSERT(m_PhysicalDevice && "Failed to find a suitable GPU!");

    QueueFamilyIndices indices = FindQueueFamilies(m_PhysicalDevice);

    VkDeviceQueueCreateInfo queueCreateInfo{};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = indices.ComputeFamily;
    queueCreateInfo.queueCount = 1;
    float queuePriority = 1.0f;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    VkPhysicalDeviceFeatures deviceFeatures{};

    std::vector<const char*> deviceExtensions;
    deviceExtensions.push_back("VK_KHR_shader_non_semantic_info");

    VkDeviceCreateInfo deviceInfo{};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pEnabledFeatures = &deviceFeatures;
    deviceInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
    deviceInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (g_EnableValidationLayers)
    {
        deviceInfo.enabledLayerCount = (uint32_t)g_ValidationLayers.size();
        deviceInfo.ppEnabledLayerNames = g_ValidationLayers.data();
    }
    else
    {
        deviceInfo.enabledLayerCount = 0;
    }

    result = vkCreateDevice(m_PhysicalDevice, &deviceInfo, nullptr, &m_Device);
    VK_CHECK(result, "Failed to create Vulkan device!");

    vkGetDeviceQueue(m_Device, indices.ComputeFamily, 0, &m_ComputeQueue);
}

void Application::SetupDescriptorAndCommandPools()
{
    VkDescriptorPoolSize poolSize{};
    poolSize.type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSize.descriptorCount = 10;

    VkDescriptorPoolCreateInfo descriptorPoolInfo{};
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.poolSizeCount = 1;
    descriptorPoolInfo.pPoolSizes = &poolSize;
    descriptorPoolInfo.maxSets = 10;

    VkResult result = vkCreateDescriptorPool(m_Device, &descriptorPoolInfo, nullptr, &m_DescriptorPool);
    VK_CHECK(result, "Failed to create Vulkan descriptor pool!");

    std::array<VkDescriptorSetLayoutBinding, 2> bindings{};
    bindings[0].binding = 0;
    bindings[0].descriptorCount = 1;
    bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bindings[0].pImmutableSamplers = nullptr;
    bindings[0].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    bindings[1].binding = 1;
    bindings[1].descriptorCount = 1;
    bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    bindings[1].pImmutableSamplers = nullptr;
    bindings[1].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;

    VkDescriptorSetLayoutCreateInfo setLayoutInfo{};
    setLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    setLayoutInfo.bindingCount = (uint32_t)bindings.size();
    setLayoutInfo.pBindings = bindings.data();

    result = vkCreateDescriptorSetLayout(m_Device, &setLayoutInfo, nullptr, &m_DescriptorSetLayout);
    VK_CHECK(result, "Failed to create Vulkan descriptor set layout!");

    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.queueFamilyIndex = FindQueueFamilies(m_PhysicalDevice).ComputeFamily;
    poolInfo.flags= VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

    result = vkCreateCommandPool(m_Device, &poolInfo, nullptr, &m_CommandPool);
    VK_CHECK(result, "Failed to create Vulkan command pool!");
}

void Application::SetupComputePipeline()
{
    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &m_DescriptorSetLayout;

    VkResult result = vkCreatePipelineLayout(m_Device, &pipelineLayoutInfo, nullptr, &m_PipelineLayout);
    VK_CHECK(result, "Failed to create Vulkan pipeline layout!");
}

bool Application::CheckValidationLayerSupport()
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
    
    for (const char* layerName : g_ValidationLayers)
    {
        bool layerFound = false;

        for (const auto& layerProperties : availableLayers)
        {
            if (strcmp(layerName, layerProperties.layerName) == 0)
            {
                layerFound = true;
                break;
            }
        }

        if (!layerFound)
        {
            return false;
        }
    }

    return true;
}

bool Application::IsDeviceSuitable(VkPhysicalDevice device)
{
    QueueFamilyIndices indices = FindQueueFamilies(device);

    return indices.IsComplete();
}

QueueFamilyIndices Application::FindQueueFamilies(VkPhysicalDevice device)
{
    QueueFamilyIndices indices{};

    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    int i = 0;
    for (const auto& queueFamily : queueFamilies)
    {
        if (queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
        {
            indices.ComputeFamily = i;
        }

        if (indices.IsComplete())
            break;

        i++;
    }

    return indices;
}

VkShaderModule Application::CreateShaderModule(const std::vector<char>& code)
{
    VkShaderModuleCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    createInfo.codeSize = code.size();
    createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkShaderModule module;
    VkResult result = vkCreateShaderModule(m_Device, &createInfo, nullptr, &module);
    VK_CHECK(result, "Failed to create Vulkan shader module!");

    return module;
}

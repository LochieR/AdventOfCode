#pragma once

#include "ExecutionResult.h"

#include <vulkan/vulkan.h>

class Application;

class DayTwo
{
public:
    DayTwo(Application* app);
    ~DayTwo();

    ExecutionResult Run();
private:
    void CreatePipeline();
    void CreateBuffers();
    void AllocateDescriptors();
private:
    Application* m_App = nullptr;

    VkShaderModule m_ShaderModule = nullptr;
    VkPipeline m_Pipeline = nullptr;
    VkDescriptorSet m_DescriptorSet = nullptr;

    VkBuffer m_InputBuffer = nullptr;
    VkDeviceMemory m_InputBufferMemory = nullptr;
    VkBuffer m_OutputBuffer = nullptr;
    VkDeviceMemory m_OutputBufferMemory = nullptr;

    size_t m_BufferSize;
};
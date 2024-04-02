#include "DayOne.h"

#include "Timer.h"
#include "Application.h"

#include <array>
#include <vector>
#include <string>
#include <fstream>

struct Line
{
    int Chars[60];

    Line()
    {
        memset(Chars, 0, sizeof(int) * 60);
    }
};

namespace Utils {

    std::vector<Line> ReadLinesFromFile(std::string_view path)
    {
        std::ifstream file(path.data());

        std::vector<std::string> lineStrings;

        std::string line;
        while (std::getline(file, line))
        {
            lineStrings.push_back(line);
        }

        file.close();

        std::vector<Line> lines;

        for (const auto& string : lineStrings)
        {
            Line line{};

            for (int i = 0; i < string.size(); i++)
            {
                line.Chars[i] = string[i];
            }

            lines.push_back(line);
        }

        return lines;
    }

}

DayOne::DayOne(Application* app)
    : m_App(app)
{
    std::vector<Line> lines = Utils::ReadLinesFromFile("inputs/Day1.txt");

    m_BufferSize = lines.size();

    CreatePipeline();
    CreateBuffers();
    AllocateDescriptors();
}

DayOne::~DayOne()
{
    vkDestroyBuffer(m_App->m_Device, m_InputBuffer, nullptr);
    vkFreeMemory(m_App->m_Device, m_InputBufferMemory, nullptr);
    vkDestroyBuffer(m_App->m_Device, m_OutputBuffer, nullptr);
    vkFreeMemory(m_App->m_Device, m_OutputBufferMemory, nullptr);

    vkDestroyShaderModule(m_App->m_Device, m_ShaderModule, nullptr);
    vkDestroyPipeline(m_App->m_Device, m_Pipeline, nullptr);
}

ExecutionResult DayOne::Run()
{
    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandBufferCount = 1;
    allocInfo.commandPool = m_App->m_CommandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    
    VkCommandBuffer commandBuffer;
    VkResult result = vkAllocateCommandBuffers(m_App->m_Device, &allocInfo, &commandBuffer);
    VK_CHECK(result, "Failed to allocate Vulkan command buffers!");

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;

    result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
    VK_CHECK(result, "Failed to begin command buffer!");

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_Pipeline);
    vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, m_App->m_PipelineLayout,
        0, 1, &m_DescriptorSet, 0, nullptr);

    vkCmdDispatch(commandBuffer, 8, 1, 1);

    result = vkEndCommandBuffer(commandBuffer);
    VK_CHECK(result, "Failed to end command buffer!");

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.signalSemaphoreCount = 0;

    float elapsedTime = 0.0f;

    Timer timer;

    result = vkQueueSubmit(m_App->m_ComputeQueue, 1, &submitInfo, nullptr);
    VK_CHECK(result, "Failed to submit to Vulkan queue!");

    vkQueueWaitIdle(m_App->m_ComputeQueue);

    elapsedTime = timer.ElapsedMillis();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    Utils::CreateBuffer(
        m_App->m_Device, m_App->m_PhysicalDevice,
        sizeof(int) * m_BufferSize * 2,
        VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,
        stagingBufferMemory
    );

    Utils::CopyBuffer(m_OutputBuffer, stagingBuffer, sizeof(int) * m_BufferSize * 2);

    std::vector<int> buffer1(m_BufferSize);
    std::vector<int> buffer2(m_BufferSize);

    void* data;
    vkMapMemory(m_App->m_Device, stagingBufferMemory, 0, sizeof(int) * m_BufferSize * 2, 0, &data);
    memcpy(buffer1.data(), data, sizeof(int) * m_BufferSize);
    memcpy(buffer2.data(), (int*)data + m_BufferSize, sizeof(int) * m_BufferSize);
    vkUnmapMemory(m_App->m_Device, stagingBufferMemory);

    vkDestroyBuffer(m_App->m_Device, stagingBuffer, nullptr);
    vkFreeMemory(m_App->m_Device, stagingBufferMemory, nullptr);

    int total1 = 0;

    for (int i : buffer1)
        total1 += i;

    int total2 = 0;

    for (int i : buffer2)
        total2 += i;
        
    ExecutionResult exeResult;
    exeResult.TimeMillis = elapsedTime;
    exeResult.PartOne.IntResult = total1;
    exeResult.PartTwo.IntResult = total2;

    return exeResult;
}

void DayOne::CreatePipeline()
{
    auto shaderData = Utils::ReadFile("Day1.comp.spv");
    m_ShaderModule = m_App->CreateShaderModule(shaderData);

    VkPipelineShaderStageCreateInfo shaderStageInfo{};
    shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
    shaderStageInfo.module = m_ShaderModule;
    shaderStageInfo.pName = "main";

    VkComputePipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
    pipelineInfo.layout = m_App->m_PipelineLayout;
    pipelineInfo.stage = shaderStageInfo;

    VkResult result = vkCreateComputePipelines(m_App->m_Device, nullptr, 1, &pipelineInfo, nullptr, &m_Pipeline);
    VK_CHECK(result, "Failed to create Vulkan pipeline!");
}

void DayOne::CreateBuffers()
{
    std::vector<Line> lines = Utils::ReadLinesFromFile("inputs/Day1.txt");

    VkDeviceSize bufferSize = sizeof(Line) * m_BufferSize;

    Utils::CreateBuffer(
        m_App->m_Device, m_App->m_PhysicalDevice,
        bufferSize,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        m_InputBuffer,
        m_InputBufferMemory
    );

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;

    Utils::CreateBuffer(
        m_App->m_Device, m_App->m_PhysicalDevice,
        bufferSize,
        VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        stagingBuffer,
        stagingBufferMemory
    );

    void* data;
    vkMapMemory(m_App->m_Device, stagingBufferMemory, 0, bufferSize, 0, &data);
    memcpy(data, lines.data(), bufferSize);
    vkUnmapMemory(m_App->m_Device, stagingBufferMemory);

    Utils::CopyBuffer(stagingBuffer, m_InputBuffer, bufferSize);

    vkDestroyBuffer(m_App->m_Device, stagingBuffer, nullptr);
    vkFreeMemory(m_App->m_Device, stagingBufferMemory, nullptr);

    Utils::CreateBuffer(
        m_App->m_Device, m_App->m_PhysicalDevice,
        sizeof(int) * m_BufferSize * 2,
        VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
        m_OutputBuffer,
        m_OutputBufferMemory
    );
}

void DayOne::AllocateDescriptors()
{
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_App->m_DescriptorPool;
    allocInfo.descriptorSetCount = 1;
    allocInfo.pSetLayouts = &m_App->m_DescriptorSetLayout;

    VkResult result = vkAllocateDescriptorSets(m_App->m_Device, &allocInfo, &m_DescriptorSet);
    VK_CHECK(result, "Failed to allocate Vulkan descriptor set!");

    std::array<VkDescriptorBufferInfo, 2> bufferInfos{};
    bufferInfos[0].buffer = m_InputBuffer;
    bufferInfos[0].offset = 0;
    bufferInfos[0].range = sizeof(Line) * m_BufferSize;

    bufferInfos[1].buffer = m_OutputBuffer;
    bufferInfos[1].offset = 0;
    bufferInfos[1].range = sizeof(int) * m_BufferSize * 2;

    std::array<VkWriteDescriptorSet, 2> descriptorWrites{};
    descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[0].dstSet = m_DescriptorSet;
    descriptorWrites[0].dstBinding = 0;
    descriptorWrites[0].dstArrayElement = 0;
    descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorWrites[0].descriptorCount = 1;
    descriptorWrites[0].pBufferInfo = &bufferInfos[0];

    descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorWrites[1].dstSet = m_DescriptorSet;
    descriptorWrites[1].dstBinding = 1;
    descriptorWrites[1].dstArrayElement = 0;
    descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    descriptorWrites[1].descriptorCount = 1;
    descriptorWrites[1].pBufferInfo = &bufferInfos[1];

    vkUpdateDescriptorSets(m_App->m_Device, 2, descriptorWrites.data(), 0, nullptr);
}

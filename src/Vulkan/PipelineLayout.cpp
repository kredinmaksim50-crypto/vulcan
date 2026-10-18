#include "PipelineLayout.h"

#include <stdexcept>

void PipelineLayout::Create(VulkanObjects& objects)
{
    VkPushConstantRange ranges[2]{};

    // Vertex shader: angleX и angleY
    ranges[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    ranges[0].offset = 0;
    ranges[0].size = 2 * sizeof(float);

    // Fragment shader: alphass
    ranges[1].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    ranges[1].offset = 2 * sizeof(float);
    ranges[1].size = sizeof(float);

    VkPipelineLayoutCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;

    createInfo.setLayoutCount = 0;
    createInfo.pSetLayouts = nullptr;

    createInfo.pushConstantRangeCount = 2;
    createInfo.pPushConstantRanges = ranges;

    if (vkCreatePipelineLayout(
        objects.device,
        &createInfo,
        nullptr,
        &objects.pipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Pipeline Layout");
    }
}

void PipelineLayout::Destroy(VulkanObjects& objects)
{
    if (objects.pipelineLayout != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(
            objects.device,
            objects.pipelineLayout,
            nullptr
        );

        objects.pipelineLayout = VK_NULL_HANDLE;
    }
}
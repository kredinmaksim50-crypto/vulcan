#include "CommandBuffers.h"
#include <stdexcept>
#include <cassert>

void CommandBuffers::Create(VulkanObjects& objects)
{
    objects.commandBuffers.resize(
        objects.swapChainFramebuffers.size());

    VkCommandBufferAllocateInfo allocateInfo{};
    allocateInfo.sType =
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;

    allocateInfo.commandPool = objects.commandPool;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount =
        static_cast<uint32_t>(objects.commandBuffers.size());

    if (vkAllocateCommandBuffers(
        objects.device,
        &allocateInfo,
        objects.commandBuffers.data()) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate Command Buffers");
    }
}

void CommandBuffers::Destroy(VulkanObjects& objects)
{
    if (!objects.commandBuffers.empty())
    {
        vkFreeCommandBuffers(
            objects.device,
            objects.commandPool,
            static_cast<uint32_t>(objects.commandBuffers.size()),
            objects.commandBuffers.data());

        objects.commandBuffers.clear();
    }
}

void CommandBuffers::Record(
    VulkanObjects& objects,
    uint32_t imageIndex,
    float angleX,
    float angleY,
    float alphass)
{
    VkCommandBuffer commandBuffer = objects.commandBuffers[imageIndex];

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS)
        throw std::runtime_error("Failed to begin recording command buffer");

    VkClearValue clearValues[2]{};

    clearValues[0].color = { {0.05f, 0.05f, 0.08f, 1.0f} };
    clearValues[1].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = objects.renderPass;
    renderPassInfo.framebuffer = objects.swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = objects.swapChainExtent;
    renderPassInfo.clearValueCount = 2;
    renderPassInfo.pClearValues = clearValues;


    struct PushConstants
    {
        float angleX;
        float angleY;
        float alphass;
    };

    PushConstants pc{ angleX, angleY, alphass };


    vkCmdBeginRenderPass(
        commandBuffer,
        &renderPassInfo,
        VK_SUBPASS_CONTENTS_INLINE);



    vkCmdBindPipeline(
        commandBuffer,
        VK_PIPELINE_BIND_POINT_GRAPHICS,
        objects.graphicsPipeline);

    vkCmdPushConstants(
        commandBuffer,
        objects.pipelineLayout,
        VK_SHADER_STAGE_VERTEX_BIT,
        0,
        2 * sizeof(float),
        &pc);

    vkCmdPushConstants(
        commandBuffer,
        objects.pipelineLayout,
        VK_SHADER_STAGE_FRAGMENT_BIT,
        2 * sizeof(float),
        sizeof(float),
        &pc.alphass);

    vkCmdDraw(commandBuffer, 42, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
        throw std::runtime_error("Failed to record command buffer");
}
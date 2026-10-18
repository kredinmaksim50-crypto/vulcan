#include "Framebuffer.h"
#include <stdexcept>

void Framebuffer::Create(VulkanObjects& objects)
{
    objects.swapChainFramebuffers.resize(
        objects.swapChainImageViews.size());

    for (size_t i = 0; i < objects.swapChainImageViews.size(); i++)
    {
        VkImageView attachments[] = {
            objects.swapChainImageViews[i],
    objects.depthImageView
        };

        VkFramebufferCreateInfo createInfo{};
        createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        createInfo.renderPass = objects.renderPass;
        createInfo.attachmentCount = 2;
        createInfo.pAttachments = attachments;
        createInfo.width = objects.swapChainExtent.width;
        createInfo.height = objects.swapChainExtent.height;
        createInfo.layers = 1;

        if (vkCreateFramebuffer(
            objects.device,
            &createInfo,
            nullptr,
            &objects.swapChainFramebuffers[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create Framebuffer");
        }
    }
}

void Framebuffer::Destroy(VulkanObjects& objects)
{
    for (VkFramebuffer framebuffer : objects.swapChainFramebuffers)
    {
        vkDestroyFramebuffer(
            objects.device,
            framebuffer,
            nullptr);
    }

    objects.swapChainFramebuffers.clear();
}
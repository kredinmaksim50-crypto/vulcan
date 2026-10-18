#include "CommandPool.h"
#include <stdexcept>

void CommandPool::Create(VulkanObjects& objects)
{
    VkCommandPoolCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    createInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    createInfo.queueFamilyIndex = objects.graphicsFamily;

    if (vkCreateCommandPool(
        objects.device,
        &createInfo,
        nullptr,
        &objects.commandPool) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Command Pool");
    }
}

void CommandPool::Destroy(VulkanObjects& objects)
{
    if (objects.commandPool != VK_NULL_HANDLE)
    {
        vkDestroyCommandPool(
            objects.device,
            objects.commandPool,
            nullptr);

        objects.commandPool = VK_NULL_HANDLE;
    }
}
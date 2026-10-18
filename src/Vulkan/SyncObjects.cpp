#include "SyncObjects.h"
#include <stdexcept>

void SyncObjects::Create(VulkanObjects& objects)
{
    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    if (vkCreateSemaphore(
        objects.device, &semaphoreInfo, nullptr,
        &objects.imageAvailableSemaphore) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create imageAvailableSemaphore");
    }

    if (vkCreateSemaphore(
        objects.device, &semaphoreInfo, nullptr,
        &objects.renderFinishedSemaphore) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create renderFinishedSemaphore");
    }

    if (vkCreateFence(
        objects.device, &fenceInfo, nullptr,
        &objects.inFlightFence) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create inFlightFence");
    }
}

void SyncObjects::Destroy(VulkanObjects& objects)
{
    if (objects.inFlightFence != VK_NULL_HANDLE)
    {
        vkDestroyFence(objects.device, objects.inFlightFence, nullptr);
        objects.inFlightFence = VK_NULL_HANDLE;
    }

    if (objects.renderFinishedSemaphore != VK_NULL_HANDLE)
    {
        vkDestroySemaphore(objects.device, objects.renderFinishedSemaphore, nullptr);
        objects.renderFinishedSemaphore = VK_NULL_HANDLE;
    }

    if (objects.imageAvailableSemaphore != VK_NULL_HANDLE)
    {
        vkDestroySemaphore(objects.device, objects.imageAvailableSemaphore, nullptr);
        objects.imageAvailableSemaphore = VK_NULL_HANDLE;
    }
}
#include "SyncObjects.h"
#include <stdexcept>

void SyncObjects::Create(VulkanObjects& objects)
{
    const int MAX_FRAMES_IN_FLIGHT = 2;

    // Сначала выделяем память в векторах под 2 комплекта объектов
    objects.imageAvailableSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
    objects.renderFinishedSemaphore.resize(MAX_FRAMES_IN_FLIGHT);
    objects.inFlightFence.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Наш открытый забор при старте

    // Цикл на 2 повторения для создания независимых наборов синхронизации
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
    {
        if (vkCreateSemaphore(
            objects.device, &semaphoreInfo, nullptr,
            &objects.imageAvailableSemaphore[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create imageAvailableSemaphore");
        }

        if (vkCreateSemaphore(
            objects.device, &semaphoreInfo, nullptr,
            &objects.renderFinishedSemaphore[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create renderFinishedSemaphore");
        }

        if (vkCreateFence(
            objects.device, &fenceInfo, nullptr,
            &objects.inFlightFence[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create inFlightFence");
        }
    }
}


void SyncObjects::Destroy(VulkanObjects& objects)
{
    // Проходим циклом по всем созданным элементам (их размер равен размеру любого из векторов)
    for (size_t i = 0; i < objects.inFlightFence.size(); i++)
    {
        if (objects.inFlightFence[i] != VK_NULL_HANDLE)
        {
            vkDestroyFence(objects.device, objects.inFlightFence[i], nullptr);
        }

        if (objects.renderFinishedSemaphore[i] != VK_NULL_HANDLE)
        {
            vkDestroySemaphore(objects.device, objects.renderFinishedSemaphore[i], nullptr);
        }

        if (objects.imageAvailableSemaphore[i] != VK_NULL_HANDLE)
        {
            vkDestroySemaphore(objects.device, objects.imageAvailableSemaphore[i], nullptr);
        }
    }

    // После удаления объектов очищаем сами векторы, возвращая их в исходное пустое состояние
    objects.inFlightFence.clear();
    objects.renderFinishedSemaphore.clear();
    objects.imageAvailableSemaphore.clear();
}

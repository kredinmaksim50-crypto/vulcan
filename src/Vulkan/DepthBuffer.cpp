#include "DepthBuffer.h"
#include <stdexcept>

void DepthBuffer::Create(VulkanObjects& objects)
{
    // 1. Создаём изображение глубины
    VkImageCreateInfo imageInfo{};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = objects.swapChainExtent.width;
    imageInfo.extent.height = objects.swapChainExtent.height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = objects.depthFormat;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    if (vkCreateImage(
        objects.device,
        &imageInfo,
        nullptr,
        &objects.depthImage) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create depth image");
    }

    // 2. Получаем требования к памяти
    VkMemoryRequirements memoryRequirements{};
    vkGetImageMemoryRequirements(
        objects.device,
        objects.depthImage,
        &memoryRequirements);

    VkPhysicalDeviceMemoryProperties memoryProperties{};
    vkGetPhysicalDeviceMemoryProperties(
        objects.physicalDevice,
        &memoryProperties);

    uint32_t memoryTypeIndex = UINT32_MAX;

    for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
    {
        if ((memoryRequirements.memoryTypeBits & (1u << i)) &&
            (memoryProperties.memoryTypes[i].propertyFlags &
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT))
        {
            memoryTypeIndex = i;
            break;
        }
    }

    if (memoryTypeIndex == UINT32_MAX)
        throw std::runtime_error("Failed to find depth image memory");

    // 3. Выделяем память
    VkMemoryAllocateInfo allocateInfo{};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = memoryTypeIndex;

    if (vkAllocateMemory(
        objects.device,
        &allocateInfo,
        nullptr,
        &objects.depthImageMemory) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to allocate depth image memory");
    }

    if (vkBindImageMemory(
        objects.device,
        objects.depthImage,
        objects.depthImageMemory,
        0) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to bind depth image memory");
    }

    // 4. Создаём представление изображения
    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = objects.depthImage;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = objects.depthFormat;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(
        objects.device,
        &viewInfo,
        nullptr,
        &objects.depthImageView) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create depth image view");
    }
}

void DepthBuffer::Destroy(VulkanObjects& objects)
{
    if (objects.depthImageView != VK_NULL_HANDLE)
    {
        vkDestroyImageView(objects.device, objects.depthImageView, nullptr);
        objects.depthImageView = VK_NULL_HANDLE;
    }

    if (objects.depthImage != VK_NULL_HANDLE)
    {
        vkDestroyImage(objects.device, objects.depthImage, nullptr);
        objects.depthImage = VK_NULL_HANDLE;
    }

    if (objects.depthImageMemory != VK_NULL_HANDLE)
    {
        vkFreeMemory(objects.device, objects.depthImageMemory, nullptr);
        objects.depthImageMemory = VK_NULL_HANDLE;
    }
}
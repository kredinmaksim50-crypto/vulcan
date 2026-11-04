#include "image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <stdexcept>
#include <iostream>

uint32_t findMemoryType(VkPhysicalDevice physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    throw std::runtime_error("Failed to find suitable memory type!");
}

void images::createBufferImage(VulkanObjects& objects)
{
    int texWidth, texHeight, texChannels;

    stbi_uc* pixels = stbi_load("E:/photo/cute-black-cat-sitting-vector_1163677-4913.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

    if (!pixels) {
        throw std::runtime_error("Failed to load texture image!");
    }

    VkDeviceSize imageSize = texWidth * texHeight * 4;

    VkBufferCreateInfo bufferInfo = {}; // это описания того как созлдатть память то есть описания размера памяти  для передачи в gpu 
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = imageSize;
    bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkBuffer stagingBuffer; // VkBuffer это описания самого ресурса памяти
    if (vkCreateBuffer(objects.device, &bufferInfo, nullptr, &stagingBuffer) != VK_SUCCESS) {
        stbi_image_free(pixels);
        throw std::runtime_error("Failed to create staging buffer!");
    }

    VkMemoryRequirements memRequirements; // vkMemoryRequirement это какого типа и размера память надо выделить физически в gpu 
    vkGetBufferMemoryRequirements(objects.device, stagingBuffer, &memRequirements);

    VkMemoryAllocateInfo allocInfo = {}; // vkmemoryallocaate это описания того как именно создать  память то есть какого размера и типа
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(objects.physicalDevice, memRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    VkDeviceMemory stagingBufferMemory; // vkDeviceMemory это физическая та самая память с начтройками 
    if (vkAllocateMemory(objects.device, &allocInfo, nullptr, &stagingBufferMemory) != VK_SUCCESS) {
        vkDestroyBuffer(objects.device, stagingBuffer, nullptr);
        stbi_image_free(pixels);
        throw std::runtime_error("Failed to allocate staging buffer memory!");
    }

    vkBindBufferMemory(objects.device, stagingBuffer, stagingBufferMemory, 0);

    void* data;
    vkMapMemory(objects.device, stagingBufferMemory, 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(objects.device, stagingBufferMemory);

    stbi_image_free(pixels);

    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = static_cast<uint32_t>(texWidth);
    imageInfo.extent.height = static_cast<uint32_t>(texHeight);
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

    if (vkCreateImage(objects.device, &imageInfo, nullptr, &objects.Image) != VK_SUCCESS) {
        vkDestroyBuffer(objects.device, stagingBuffer, nullptr);
        vkFreeMemory(objects.device, stagingBufferMemory, nullptr);
        throw std::runtime_error("Failed to create image!");
    }

    VkMemoryRequirements imgMemRequirements;
    vkGetImageMemoryRequirements(objects.device, objects.Image, &imgMemRequirements);

    VkMemoryAllocateInfo imgAllocInfo = {};
    imgAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    imgAllocInfo.allocationSize = imgMemRequirements.size;
    imgAllocInfo.memoryTypeIndex = findMemoryType(objects.physicalDevice, imgMemRequirements.memoryTypeBits,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    if (vkAllocateMemory(objects.device, &imgAllocInfo, nullptr, &objects.DeviceMemory) != VK_SUCCESS) {
        vkDestroyImage(objects.device, objects.Image, nullptr);
        vkDestroyBuffer(objects.device, stagingBuffer, nullptr);
        vkFreeMemory(objects.device, stagingBufferMemory, nullptr);
        throw std::runtime_error("Failed to allocate image memory!");
    }

    vkBindImageMemory(objects.device, objects.Image, objects.DeviceMemory, 0);





    VkImageMemoryBarrier2 transitionToDstBarrier = {}; 
    transitionToDstBarrier.image = objects.Image;
    transitionToDstBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    transitionToDstBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    transitionToDstBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    transitionToDstBarrier.srcStageMask = VK_PIPELINE_STAGE_2_NONE;
    transitionToDstBarrier.srcAccessMask = VK_ACCESS_2_NONE;
    transitionToDstBarrier.dstStageMask = VK_PIPELINE_STAGE_2_COPY_BIT;
    transitionToDstBarrier.dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    transitionToDstBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    transitionToDstBarrier.subresourceRange.baseMipLevel = 0;
    transitionToDstBarrier.subresourceRange.levelCount = 1;
    transitionToDstBarrier.subresourceRange.baseArrayLayer = 0;
    transitionToDstBarrier.subresourceRange.layerCount = 1;

    VkDependencyInfo depInfoToDst = {};
    depInfoToDst.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    depInfoToDst.imageMemoryBarrierCount = 1;
    depInfoToDst.pImageMemoryBarriers = &transitionToDstBarrier;


    VkImageMemoryBarrier2 transitionToShaderBarrier = {};
    transitionToShaderBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
    transitionToShaderBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    transitionToShaderBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    transitionToShaderBarrier.srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT;
    transitionToShaderBarrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
    transitionToShaderBarrier.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
    transitionToShaderBarrier.dstAccessMask = VK_ACCESS_2_SHADER_SAMPLED_READ_BIT;
    transitionToShaderBarrier.image = objects.Image;
    transitionToShaderBarrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    transitionToShaderBarrier.subresourceRange.baseMipLevel = 0;
    transitionToShaderBarrier.subresourceRange.levelCount = 1;
    transitionToShaderBarrier.subresourceRange.baseArrayLayer = 0;
    transitionToShaderBarrier.subresourceRange.layerCount = 1;

    VkDependencyInfo depInfoToShader = {};
    depInfoToShader.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
    depInfoToShader.imageMemoryBarrierCount = 1;
    depInfoToShader.pImageMemoryBarriers =
        &transitionToShaderBarrier;


    VkBufferImageCopy region = {};
    region.bufferOffset = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.layerCount = 1;
    region.imageExtent = { static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1 };

    VkCommandBufferAllocateInfo cmdAllocInfo = {};
    cmdAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdAllocInfo.commandPool = objects.commandPool;
    cmdAllocInfo.commandBufferCount = 1;

    VkCommandBuffer tempCommandBuffer;
    vkAllocateCommandBuffers(objects.device, &cmdAllocInfo, &tempCommandBuffer);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(tempCommandBuffer, &beginInfo);
    vkCmdPipelineBarrier2(tempCommandBuffer, &depInfoToDst);
    vkCmdCopyBufferToImage(tempCommandBuffer, stagingBuffer, objects.Image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);
    vkCmdPipelineBarrier2(tempCommandBuffer, &depInfoToShader);
    vkEndCommandBuffer(tempCommandBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &tempCommandBuffer;

    vkQueueSubmit(objects.graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(objects.graphicsQueue);

    vkFreeCommandBuffers(objects.device, objects.commandPool, 1, &tempCommandBuffer);

    vkDestroyBuffer(objects.device, stagingBuffer, nullptr);
    vkFreeMemory(objects.device, stagingBufferMemory, nullptr);

    VkImageViewCreateInfo viewInfo = {};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.image = objects.Image;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = VK_FORMAT_R8G8B8A8_SRGB;
    viewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    if (vkCreateImageView(objects.device, &viewInfo, nullptr, &objects.textureImageView) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create image view!");
    }

    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = 16.0f;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;
    if (vkCreateSampler(objects.device, &samplerInfo, nullptr, &objects.textureSampler) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create sampler!");
    }
}
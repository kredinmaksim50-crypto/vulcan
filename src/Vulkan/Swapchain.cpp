#include "Swapchain.h"

#include <vector>
#include <stdexcept>
#include <algorithm>
#include <cstdint>

void Swapchain::Create(VulkanObjects& objects)
{
    // 1. Получаем характеристики поверхности
    VkSurfaceCapabilitiesKHR capabilities{};

    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(objects.physicalDevice, objects.surface, &capabilities) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to get surface capabilities");
    }

    // 2. Получаем доступные форматы изображений
    uint32_t formatCount = 0;

    vkGetPhysicalDeviceSurfaceFormatsKHR(objects.physicalDevice, objects.surface, &formatCount, nullptr);

    if (formatCount == 0)
        throw std::runtime_error("No surface formats available");

    std::vector<VkSurfaceFormatKHR> formats(formatCount);

    vkGetPhysicalDeviceSurfaceFormatsKHR(objects.physicalDevice, objects.surface, &formatCount, formats.data());

    // 3. Получаем доступные режимы отображения
    uint32_t presentModeCount = 0;

    vkGetPhysicalDeviceSurfacePresentModesKHR(objects.physicalDevice, objects.surface, &presentModeCount, nullptr);

    if (presentModeCount == 0)
        throw std::runtime_error("No present modes available");

    std::vector<VkPresentModeKHR> presentModes(presentModeCount);

    vkGetPhysicalDeviceSurfacePresentModesKHR(objects.physicalDevice, objects.surface, &presentModeCount,  presentModes.data());

    VkSurfaceFormatKHR surfaceFormat = formats[0];

    VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;

    VkExtent2D extent = capabilities.currentExtent;

    if (extent.width == UINT32_MAX)
    {
        extent.width = 1000;
        extent.height = 1000;

        extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);

        extent.height = std::clamp( extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
    }

    uint32_t imageCount = capabilities.minImageCount + 1;

    if (capabilities.maxImageCount > 0 &&
        imageCount > capabilities.maxImageCount)
    {
        imageCount = capabilities.maxImageCount;
    }

    VkSwapchainCreateInfoKHR createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = objects.surface;
    createInfo.minImageCount = imageCount;
    createInfo.imageFormat = surfaceFormat.format;
    createInfo.imageColorSpace = surfaceFormat.colorSpace;
    createInfo.imageExtent = extent;
    createInfo.imageArrayLayers = 1;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    uint32_t queueFamilyIndices[] = {
        objects.graphicsFamily,
        objects.presentFamily
    };

    if (objects.graphicsFamily != objects.presentFamily)
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        createInfo.queueFamilyIndexCount = 2;
        createInfo.pQueueFamilyIndices = queueFamilyIndices;
    }
    else
    {
        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;
    }

    createInfo.preTransform = capabilities.currentTransform;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = presentMode;
    createInfo.clipped = VK_TRUE;
    createInfo.oldSwapchain = VK_NULL_HANDLE;

    if (vkCreateSwapchainKHR(
        objects.device,
        &createInfo,
        nullptr,
        &objects.swapChain) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Swapchain");
    }

    objects.swapChainImageFormat = surfaceFormat.format;
    objects.swapChainExtent = extent;

    uint32_t actualImageCount = 0;

    vkGetSwapchainImagesKHR(
        objects.device,
        objects.swapChain,
        &actualImageCount,
        nullptr
    );

    objects.swapChainImages.resize(actualImageCount);

    vkGetSwapchainImagesKHR(
        objects.device,
        objects.swapChain,
        &actualImageCount,
        objects.swapChainImages.data()
    );
}

void Swapchain::Destroy(VulkanObjects& objects)
{
    if (objects.swapChain != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(objects.device, objects.swapChain, nullptr);
        objects.swapChain = VK_NULL_HANDLE;
    }
}
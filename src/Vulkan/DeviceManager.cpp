#include "DeviceManager.h"

#include <vector>
#include <stdexcept>

void DeviceManager::SelectPhysicalDevice(VulkanObjects& objects)
{
    uint32_t deviceCount = 0;

    vkEnumeratePhysicalDevices(objects.instance, &deviceCount, nullptr);

    if (deviceCount == 0)
    {
        throw std::runtime_error("No Vulkan-compatible GPU found");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);

    vkEnumeratePhysicalDevices(objects.instance, &deviceCount, devices.data());

    objects.physicalDevice = devices[0];
}

void DeviceManager::CreateLogicalDevice(VulkanObjects& objects)
{
    uint32_t queueFamilyCount = 0;

    vkGetPhysicalDeviceQueueFamilyProperties(objects.physicalDevice, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);

    vkGetPhysicalDeviceQueueFamilyProperties(objects.physicalDevice, &queueFamilyCount, queueFamilies.data());


    uint32_t graphicsFamily = UINT32_MAX;
    uint32_t presentFamily = UINT32_MAX;

    for (uint32_t i = 0; i < queueFamilyCount; i++)
    {
        if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
            graphicsFamily = i;

        VkBool32 presentSupport = false;

        vkGetPhysicalDeviceSurfaceSupportKHR(objects.physicalDevice, i, objects.surface, &presentSupport); // если сечейство умеет выводить на экран

        if (presentSupport)
            presentFamily = i;

        if (graphicsFamily != UINT32_MAX &&
            presentFamily != UINT32_MAX)
            break;
    }

    if (graphicsFamily == UINT32_MAX ||
        presentFamily == UINT32_MAX)
    {
        throw std::runtime_error("Required queue families not found");
    }


    float queuePriority = 1.0f;

    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;

    std::vector<uint32_t> uniqueFamilies = {
        graphicsFamily,
        presentFamily
    };

    if (graphicsFamily == presentFamily)
        uniqueFamilies.resize(1);

    for (uint32_t family : uniqueFamilies)
    {
        VkDeviceQueueCreateInfo queueInfo{};
        queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.queueFamilyIndex = family;
        queueInfo.queueCount = 1;
        queueInfo.pQueuePriorities = &queuePriority;

        queueCreateInfos.push_back(queueInfo);
    }



    VkPhysicalDeviceFeatures deviceFeatures{};

    VkDeviceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    createInfo.pQueueCreateInfos = queueCreateInfos.data();
    createInfo.pEnabledFeatures = &deviceFeatures;

    const char* deviceExtensions[] = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
    };

    createInfo.enabledExtensionCount = 1;
    createInfo.ppEnabledExtensionNames = deviceExtensions;

    if (vkCreateDevice(objects.physicalDevice, &createInfo, nullptr, &objects.device) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create logical device");
    }

    objects.graphicsFamily = graphicsFamily;
    objects.presentFamily = presentFamily;

    vkGetDeviceQueue(objects.device, graphicsFamily, 0, &objects.graphicsQueue);

    vkGetDeviceQueue(objects.device, presentFamily, 0, &objects.presentQueue);
}
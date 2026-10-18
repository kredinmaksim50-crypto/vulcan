#include "VulkanContext.h"
#include "../Application/Window/Window.h"
#include <stdexcept>
#include <vector>

void VulkanContext::Initialize(Window& window, VulkanObjects& objects)
{
    // 1. Получаем расширения GLFW
    uint32_t glfwExtensionCount = 0;
    const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    // 2. Информация о приложении
    VkApplicationInfo appInfo{};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Vulkan";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "My Engine";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_4;

    // 3. Информация о создании Instance
    VkInstanceCreateInfo createInfo{};
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = glfwExtensionCount;
    createInfo.ppEnabledExtensionNames = glfwExtensions;

    // 4. Создаём Instance
    if (vkCreateInstance(&createInfo, nullptr, &objects.instance) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Vulkan Instance");
    }

    // 5. Создаём Surface
    if (glfwCreateWindowSurface(
        objects.instance,
        window.GetHandle(),
        nullptr,
        &objects.surface) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to create Vulkan Surface");
    }
}

void VulkanContext::Destroy()
{
    if (objects.surface != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(objects.instance, objects.surface, nullptr);
        objects.surface = VK_NULL_HANDLE;
    }

    if (objects.instance != VK_NULL_HANDLE)
    {
        vkDestroyInstance(objects.instance, nullptr);
        objects.instance = VK_NULL_HANDLE;
    }
}
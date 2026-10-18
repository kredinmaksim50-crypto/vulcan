#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "VulkanObjects.h"

class Window;

class VulkanContext
{
public:
    void Initialize(Window& window, VulkanObjects& objects);
    void Destroy();

    VulkanObjects& GetObjects()
    {
        return objects;
    }

private:
    VulkanObjects objects;
};
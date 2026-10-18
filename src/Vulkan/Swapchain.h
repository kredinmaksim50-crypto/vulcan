#pragma once

#include "VulkanObjects.h"

class Swapchain
{
public:
    void Create(VulkanObjects& objects);
    void Destroy(VulkanObjects& objects);
};
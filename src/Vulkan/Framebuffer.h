#pragma once

#include "VulkanObjects.h"

class Framebuffer
{
public:
    void Create(VulkanObjects& objects);
    void Destroy(VulkanObjects& objects);
};
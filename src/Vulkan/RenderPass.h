#pragma once

#include "VulkanObjects.h"

class RenderPass
{
public:
    void Create(VulkanObjects& objects);
    void Destroy(VulkanObjects& objects);
};
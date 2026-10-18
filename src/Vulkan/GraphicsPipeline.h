#pragma once

#include "VulkanObjects.h"

class GraphicsPipeline
{
public:
    void Create(VulkanObjects& objects);
    void Destroy(VulkanObjects& objects);
};
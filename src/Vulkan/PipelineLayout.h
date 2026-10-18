#pragma once

#include "VulkanObjects.h"

class PipelineLayout
{
public:
    void Create(VulkanObjects& objects);
    void Destroy(VulkanObjects& objects);
};
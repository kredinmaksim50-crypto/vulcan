
#pragma once
#include "VulkanObjects.h"

class DepthBuffer
{
public:
    void Create(VulkanObjects& objects);
    void Destroy(VulkanObjects& objects);
};
#pragma once
#include "VulkanObjects.h"

class CommandBuffers
{
public:
    void Create(VulkanObjects& objects);
    void Destroy(VulkanObjects& objects);

    void Record(
        VulkanObjects& objects,
        uint32_t imageIndex,
        float angleX,
        float angleY,
        float alphass);
};
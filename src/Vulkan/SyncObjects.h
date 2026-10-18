#pragma once
#include "VulkanObjects.h"

class SyncObjects
{
public:
    void Create(VulkanObjects& objects);
    void Destroy(VulkanObjects& objects);
};
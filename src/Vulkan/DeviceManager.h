#pragma once

#include "VulkanObjects.h"

class DeviceManager
{
public:
    void SelectPhysicalDevice(VulkanObjects& objects);
    void CreateLogicalDevice(VulkanObjects& objects);
};
#pragma once
#include "Window/Window.h"
#include "../Vulkan/VulkanContext.h"
#include "../Vulkan/DeviceManager.h"
#include "../Vulkan/Swapchain.h"
#include "../Vulkan/ImageViews.h"
#include "../Vulkan/RenderPass.h"
#include "../Vulkan/PipelineLayout.h"
#include "../Vulkan/GraphicsPipeline.h"
#include "../Vulkan/Framebuffer.h"
#include "../Vulkan/CommandPool.h"
#include "../Vulkan/CommandBuffers.h"
#include "../Vulkan/SyncObjects.h"
#include "../Vulkan/DepthBuffer.h"

class Application
{
public:
    void Run();
    void DrawFrame();

private:
    float angleX = 0.0f;
    float angleY = 0.0f;

    VulkanObjects vulkanObjects;

    Window window;
    VulkanContext vulkanContext;
    DeviceManager deviceManager;

    Swapchain swapchain;
    ImageViews imageViews;
    RenderPass renderPass;
    PipelineLayout pipelineLayout;
    GraphicsPipeline graphicsPipeline;
    Framebuffer framebuffer;
    CommandPool commandPool;
    CommandBuffers commandBuffers;
    SyncObjects syncObjects;
    DepthBuffer depthBuffer;
};
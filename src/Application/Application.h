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
#include "../MVP/p/Perspective.h"
#include "../Vulkan/UniformBufferObject.h"
#include "../MVP/math/mathfff.cpp"
#include "../Vulkan/Descriptions.h"
#include "../Vulkan/image.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Application
{
public:
    void Run();
    void DrawFrame();

private:

    int currentFrame = 0;
    const int MAX_FRAMES_IN_FLIGHT = 2;

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
    UniformBufferObjects uniformBufferObjects;
    images img;

};
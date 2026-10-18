#include "Application.h"
#include <iostream>

void Application::Run()
{
    window.Create();
    vulkanContext.Initialize(window, vulkanObjects);
    deviceManager.SelectPhysicalDevice(vulkanObjects);
    deviceManager.CreateLogicalDevice(vulkanObjects);
    swapchain.Create(vulkanObjects);
    imageViews.Create(vulkanObjects);
    depthBuffer.Create(vulkanObjects);
    renderPass.Create(vulkanObjects);
    pipelineLayout.Create(vulkanObjects);
    graphicsPipeline.Create(vulkanObjects);
    framebuffer.Create(vulkanObjects);
    commandPool.Create(vulkanObjects);
    commandBuffers.Create(vulkanObjects);
    syncObjects.Create(vulkanObjects);




    while (!glfwWindowShouldClose(window.GetHandle()))
    {
        glfwPollEvents();
        DrawFrame();

        if (glfwGetKey(window.GetHandle(), GLFW_KEY_UP) == GLFW_PRESS)
            angleX -= 0.02f;

        if (glfwGetKey(window.GetHandle(), GLFW_KEY_DOWN) == GLFW_PRESS)
            angleX += 0.02f;

        if (glfwGetKey(window.GetHandle(), GLFW_KEY_LEFT) == GLFW_PRESS)
            angleY += 0.02f;

        if (glfwGetKey(window.GetHandle(), GLFW_KEY_RIGHT) == GLFW_PRESS)
            angleY -= 0.02f;
    }

    vkDeviceWaitIdle(vulkanObjects.device);



    syncObjects.Destroy(vulkanObjects);

    graphicsPipeline.Destroy(vulkanObjects);
    commandBuffers.Destroy(vulkanObjects);
    commandPool.Destroy(vulkanObjects);

    pipelineLayout.Destroy(vulkanObjects);
    framebuffer.Destroy(vulkanObjects);
    depthBuffer.Destroy(vulkanObjects);
    renderPass.Destroy(vulkanObjects);
    imageViews.Destroy(vulkanObjects);
    swapchain.Destroy(vulkanObjects);

    vkDestroyDevice(vulkanObjects.device, nullptr);
    vulkanObjects.device = VK_NULL_HANDLE;

   
    vulkanContext.Destroy();
    window.Destroy();
}

void Application::DrawFrame()
{
    vkWaitForFences(
        vulkanObjects.device,
        1,
        &vulkanObjects.inFlightFence,
        VK_TRUE,
        UINT64_MAX
    );

    uint32_t imageIndex = 0;

    VkResult result = vkAcquireNextImageKHR(
        vulkanObjects.device,
        vulkanObjects.swapChain,
        UINT64_MAX,
        vulkanObjects.imageAvailableSemaphore,
        VK_NULL_HANDLE,
        &imageIndex
    );

    if (result != VK_SUCCESS)
        return;

    vkResetFences(
        vulkanObjects.device,
        1,
        &vulkanObjects.inFlightFence
    );

    vkResetCommandBuffer(vulkanObjects.commandBuffers[imageIndex], 0);

    commandBuffers.Record(
        vulkanObjects,
        imageIndex,
        angleX,
        angleY,
        1.0f
    );

    VkPipelineStageFlags waitStage =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores =
        &vulkanObjects.imageAvailableSemaphore;
    submitInfo.pWaitDstStageMask = &waitStage;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers =
        &vulkanObjects.commandBuffers[imageIndex];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores =
        &vulkanObjects.renderFinishedSemaphore;

    if (vkQueueSubmit(
        vulkanObjects.graphicsQueue,
        1,
        &submitInfo,
        vulkanObjects.inFlightFence
    ) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to submit draw command");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores =
        &vulkanObjects.renderFinishedSemaphore;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &vulkanObjects.swapChain;
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(
        vulkanObjects.presentQueue,
        &presentInfo
    );
}
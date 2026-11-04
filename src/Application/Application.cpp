#include "Application.h"
#include <iostream>
#include <glm/glm.hpp>

FlyCamera camera;

float lastX = 400.0f;  // Старая X-координата мыши (центр экрана по умолчанию)
float lastY = 300.0f;  // Старая Y-координата мыши
bool firstMouse = true;

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn) {
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    // ИСПРАВЛЕНИЕ: При самом первом движении мыши просто запоминаем её позицию 
    // и выходим из функции, чтобы не было гигантского прыжка камеры!
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
        return; // <-- ОБЯЗАТЕЛЬНО ДОБАВИТЬ ЭТОТ ВОЗВРАТ
    }

    float xoffset = xpos - lastX;
    float yoffset = ypos - lastY ; // Перевернуто, так как координаты Y идут сверху вниз
    lastX = xpos;
    lastY = ypos;

    xoffset *= camera.sensitivity;
    yoffset *= camera.sensitivity;

    camera.yaw += xoffset;
    camera.pitch += yoffset;

    // Ограничиваем наклон головы вверх/вниз
    if (camera.pitch > 89.0f)  camera.pitch = 89.0f;
    if (camera.pitch < -89.0f) camera.pitch = -89.0f;

    // Пересчитываем направление взгляда
    glm::vec3 direction;
    direction.x = cos(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));
    direction.y = sin(glm::radians(camera.pitch));
    direction.z = sin(glm::radians(camera.yaw)) * cos(glm::radians(camera.pitch));

    camera.front = glm::normalize(direction);
    camera.right = glm::normalize(glm::cross(camera.front, camera.worldUp)); // ИСПРАВЛЕНИЕ: здесь worldUp вместо up
    camera.up = glm::normalize(glm::cross(camera.right, camera.front));
}


void processCameraInput(GLFWwindow* window, float deltaTime) {
    // Скорость движения, привязанная ко времени кадра (deltaTime)
    float velocity = camera.moveSpeed * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.position += camera.front * velocity; // Вперед
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.position -= camera.front * velocity; // Назад
    if (glfwGetKey(window, GLFW_KEY_A) == VK_TRUE || glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.position -= camera.right * velocity; // Влево
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.position += camera.right * velocity; // Вправо

    // Кнопки для взлета и спуска (Пробел и Левый Ctrl)
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        camera.position += camera.worldUp * velocity;
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS)
        camera.position -= camera.worldUp * velocity;
};



void Application::Run()
{
   

    Descriptions descriptions;

    window.Create();
    glfwSetInputMode(window.GetHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Прячем курсор
    glfwSetCursorPosCallback(window.GetHandle(), mouse_callback);
    vulkanContext.Initialize(window, vulkanObjects);
    deviceManager.SelectPhysicalDevice(vulkanObjects);
    deviceManager.CreateLogicalDevice(vulkanObjects);
    swapchain.Create(vulkanObjects);
    uniformBufferObjects.Create(vulkanObjects);
    imageViews.Create(vulkanObjects);
    depthBuffer.Create(vulkanObjects);
    renderPass.Create(vulkanObjects);

    descriptions.CreateDescriptorSetLayout(vulkanObjects);

    pipelineLayout.Create(vulkanObjects);
    graphicsPipeline.Create(vulkanObjects);
    framebuffer.Create(vulkanObjects);
   

   
    commandPool.Create(vulkanObjects);
    img.createBufferImage(vulkanObjects);
    descriptions.CreateDescriptorPoolAndSets(vulkanObjects);
    
    commandBuffers.Create(vulkanObjects);
    syncObjects.Create(vulkanObjects);


    float lastFrame = 0.0f;

    camera.position = glm::vec3(0.0f, 0.0f, 4.0f); // Встаем чуть поодаль
    camera.front = glm::vec3(0.0f, 0.0f, -1.0f); // Смотрим строго вглубь экрана
    camera.up = glm::vec3(0.0f, 1.0f, 0.0f);

    while (!glfwWindowShouldClose(window.GetHandle()))
    {
        glfwPollEvents();
        DrawFrame();

        if (glfwGetKey(window.GetHandle(), GLFW_KEY_ESCAPE) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window.GetHandle(), GLFW_TRUE);
        }

        if (glfwGetKey(window.GetHandle(), GLFW_KEY_UP) == GLFW_PRESS)
            angleX -= 0.02f;

        if (glfwGetKey(window.GetHandle(), GLFW_KEY_DOWN) == GLFW_PRESS)
            angleX += 0.02f;

        if (glfwGetKey(window.GetHandle(), GLFW_KEY_LEFT) == GLFW_PRESS)
            angleY += 0.02f;

        if (glfwGetKey(window.GetHandle(), GLFW_KEY_RIGHT) == GLFW_PRESS)
            angleY -= 0.02f;

        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 2. Опрашиваем клавиатуру
        processCameraInput(window.GetHandle(), deltaTime);

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
    

    vkWaitForFences(vulkanObjects.device, 1, &vulkanObjects.inFlightFence[currentFrame], VK_TRUE, UINT64_MAX);

    int width, height;
    glfwGetFramebufferSize(window.GetHandle(), &width, &height);

    if (width > 0 && height > 0) {
        uniformBufferObjects.Update(vulkanObjects, static_cast<float>(width), static_cast<float>(height));
    }

    uint32_t imageIndex = 0;

    VkResult result = vkAcquireNextImageKHR(
        vulkanObjects.device,
        vulkanObjects.swapChain,
        UINT64_MAX,
        vulkanObjects.imageAvailableSemaphore[currentFrame],
        VK_NULL_HANDLE,
        &imageIndex
    );

    if (result != VK_SUCCESS)
        return;

    vkResetFences(
        vulkanObjects.device,
        1,
        &vulkanObjects.inFlightFence[currentFrame]
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
        &vulkanObjects.imageAvailableSemaphore[currentFrame];
    submitInfo.pWaitDstStageMask = &waitStage;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers =
        &vulkanObjects.commandBuffers[imageIndex];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores =
        &vulkanObjects.renderFinishedSemaphore[currentFrame];

    if (vkQueueSubmit(
        vulkanObjects.graphicsQueue,
        1,
        &submitInfo,
        vulkanObjects.inFlightFence[currentFrame]
    ) != VK_SUCCESS)
    {
        throw std::runtime_error("Failed to submit draw command");
    }

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores =
        &vulkanObjects.renderFinishedSemaphore[currentFrame];
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &vulkanObjects.swapChain;
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(
        vulkanObjects.presentQueue,
        &presentInfo
    );

    currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}
#pragma once


#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

class Window
{
public:
    void Create();
    void Destroy();

    GLFWwindow* GetHandle() const;

private:
    GLFWwindow* window = nullptr;
};
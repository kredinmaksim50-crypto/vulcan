#include "Window.h"

void Window::Create()
{
    glfwInit();

    glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(800, 800, "Vulkan", nullptr, nullptr);
}

void Window::Destroy()
{
    if (window)
    {
        glfwDestroyWindow(window);
        window = nullptr;
    }

    glfwTerminate();
}

GLFWwindow* Window::GetHandle() const
{
    return window;
}
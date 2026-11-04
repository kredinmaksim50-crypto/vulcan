#pragma once
#include <vulkan/vulkan.h>
#define GLM_FORCE_DEPTH_ZERO_TO_ONE 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Vulkan/VulkanContext.h" // Убедитесь, что этот путь правильный к вашему VulkanObjects
#include <stdexcept>

// 📦 Наша структура матриц в шейдере
struct UniformBufferObject {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

// ⌨️ 🖱️ Наша летающая камера
struct FlyCamera {
    // Отодвигаем камеру чуть дальше назад по оси Z (на 6 единиц) и приподнимем (на 1 единицу)
    glm::vec3 position = glm::vec3(0.0f, 1.0f, 6.0f);
    glm::vec3 front = glm::vec3(0.0f, -0.2f, -1.0f); // Слегка наклоним взгляд вниз к кубу
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

    float yaw = -90.0f; // Смотрим строго по направлению оси -Z (вглубь экрана)
    float pitch = 0.0f;

    float moveSpeed = 4.0f;
    float sensitivity = 0.05f; // Уменьшим чувствительность, чтобы камера не дергалась
};


// Делаем камеру доступной для других файлов
extern FlyCamera camera;

class UniformBufferObjects {
public:
    // Функция создания физического буфера на видеокарте
    inline void Create(VulkanObjects& objects) {
        VkDeviceSize bufferSize = sizeof(UniformBufferObject);

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = bufferSize;
        bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        if (vkCreateBuffer(objects.device, &bufferInfo, nullptr, &objects.uniformBuffer) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create VkBuffer for UBO!");
        }

        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(objects.device, objects.uniformBuffer, &memRequirements);

        VkMemoryAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;

        allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, objects.physicalDevice);

        if (vkAllocateMemory(objects.device, &allocInfo, nullptr, &objects.uniformBufferMemory) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate VkDeviceMemory for UBO!");
        }

        vkBindBufferMemory(objects.device, objects.uniformBuffer, objects.uniformBufferMemory, 0);
        vkMapMemory(objects.device, objects.uniformBufferMemory, 0, bufferSize, 0, &objects.uniformBufferMapped);
    }

    // Функция обновления матриц на основе положения камеры
        // Функция обновления матриц на основе положения камеры в UniformBufferObject.h
    inline void Update(VulkanObjects& objects, float width, float height) {
        UniformBufferObject ubo{};

        // 1. Матрица объекта (куб в центре)
        ubo.model = glm::mat4(1.0f);

        // 2. ПОДКЛЮЧАЕМ СВОБОДНУЮ КАМЕРУ! 
        // Теперь ubo.view берет живые координаты, которые меняются от мышки и WASD
        ubo.view = glm::lookAt(camera.position, camera.position + camera.front, camera.up);

        // 3. Ваша работающая перспектива
        ubo.proj = glm::perspective(glm::radians(45.0f), width / height, 0.1f, 100.0f);

        // 4. Оставляем инверсию оси Y закомментированной (раз с ней работало, не трогаем!)
        // ubo.proj *= -1; 

        // 5. Отправляем данные на видеокарту
        memcpy(objects.uniformBufferMapped, &ubo, sizeof(ubo));
    }



private:
    // Вспомогательная функция поиска типа памяти
    inline uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties, VkPhysicalDevice physicalDevice) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }
        throw std::runtime_error("Failed to find suitable memory type!");
    }
};

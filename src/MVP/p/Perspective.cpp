#include "Perspective.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

glm::mat4 Perspective::getMatrixProjection(float width, float height)
{
	glm::mat4 proj = glm::perspective(glm::radians(45.0f), width / height, 0.1f, 100.0f);
	proj[1][1] *= -1;
	return proj;

}
#pragma once 

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Perspective {
	glm::mat4 getMatrixProjection(float width, float height);

};
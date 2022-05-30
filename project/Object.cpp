#include "Object.h"
#include <vector>
#include <string>

#include "GL/glew.h"
#include "glm/glm.hpp"

Object::Object(glm::vec3 position_, glm::vec3 scale_, Mesh mesh_, Texture2D texture_) {
	position = position_;
	scale = scale_;
	mesh = mesh_;
	texture = texture_;

}

Object::Object() {}
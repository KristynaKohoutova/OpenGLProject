#ifndef OBJECT

#include <iostream>
#include <sstream>
#include <string>

#include "GL/glew.h"
#include "glm/glm.hpp"

#include "Texture2D.h"
#include "Mesh.h"

class Object
{

public: 
	glm::vec3 position;
	glm::vec3 scale;
	Mesh mesh;
	Texture2D texture;
	Object();
	Object(glm::vec3 position_, glm::vec3 scale_, Mesh mesh_, Texture2D texture_);

};

#endif

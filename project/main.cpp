#include <iostream>
#include <sstream>
#include <string>
#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "ShaderProgram.h"
#include "Texture2D.h"
#include "Camera.h"
#include "Object.h"
#include "Mesh.h"

// Global Variables
const char* APP_TITLE = "OpenGL project";
int gWindowWidth = 1024;
int gWindowHeight = 768;
GLFWwindow* gWindow = NULL;
bool gWireframe = false;
int gFlashlightOn = true;
bool fullscreen = false;

const double ZOOM_SENSITIVITY = -3.0;
const float MOVE_SPEED = 5.0; // units per second
const float MOUSE_SENSITIVITY = 0.1f;

FPSCamera fpsCamera(glm::vec3(0.0f, 3.0f, 10.0f), glm::vec3(1.0, 1.0, 1.0));

void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode);
void glfw_onFramebufferSize(GLFWwindow* window, int width, int height);
void glfw_onMouseScroll(GLFWwindow* window, double deltaX, double deltaY);
void update(double elapsedTime);
void showFPS(GLFWwindow* window);
bool initOpenGL();
void fullScreen();

const int block_count = 120;
const int inside_blocks = 10;
const int count = block_count + 4 + inside_blocks;
Object objects[count];
//---------------------------------------------------------------------
// MAIN
//---------------------------------------------------------------------
int main() {

	if (!initOpenGL()) {
		std::cerr << "GLFW initialization failed" << std::endl;
		return -1;
	}

	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// Directional light
	ShaderProgram lighting;
	lighting.loadShaders("shaders/lighting.vert", "shaders/lighting.frag");

	// All the objects used in the scene
	Mesh mesh_cube;
	mesh_cube.loadOBJ("textures/crate.obj");
	Mesh mesh_floor;
	mesh_floor.loadOBJ("textures/floor.obj");
	Mesh mesh_robot;
	mesh_robot.loadOBJ("textures/robot.obj");
	Mesh mesh_cube2;
	mesh_cube2.loadOBJ("textures/woodcrate.obj");
	Texture2D texture_cube = Texture2D();
	texture_cube.loadTexture("textures/crate.jpg", true);
	Texture2D texture_floor = Texture2D();
	texture_floor.loadTexture("textures/tile_floor.jpg", true);
	Texture2D texture_robot = Texture2D();
	texture_robot.loadTexture("textures/tile_floor.jpg", true);
	Texture2D texture_cube2 = Texture2D();
	texture_cube2.loadTexture("textures/woodcrate_diffuse.jpg", true);

	// creating objects to show
	// cube wall
	for (int i = 0; i < block_count /4; i++){

		objects[i] = Object(glm::vec3(2 * i, 1, 0), glm::vec3(1.0f, 1.0f, 1.0f), mesh_cube, texture_cube);
		objects[i + block_count/4] = Object(glm::vec3(-2, 1, 2 * i), glm::vec3(1.0f, 1.0f, 1.0f), mesh_cube, texture_cube);
		objects[i + block_count/2] = Object(glm::vec3(block_count/2-2, 1, 2 * i), glm::vec3(1.0f, 1.0f, 1.0f), mesh_cube, texture_cube);
		objects[i + block_count*3/4] = Object(glm::vec3(2 * i, 1, block_count / 2 - 2), glm::vec3(1.0f, 1.0f, 1.0f), mesh_cube, texture_cube);
	}
	// floor
	objects[block_count + 1] = Object(glm::vec3(28, 1, 24), glm::vec3(7.0f, 1.0f, 7.0f), mesh_floor, texture_floor);
	// robot
	objects[block_count + 2] = Object(glm::vec3(24, 1, 24), glm::vec3(1.0f, 1.0f, 1.0f), mesh_robot, texture_robot);
	// cube - with more details - the robot is walking around it
	objects[block_count + 3] = Object(glm::vec3(28, 1, 28), glm::vec3(1.0f, 1.0f, 1.0f), mesh_cube2, texture_cube2);
	// cubes in the middle
	int positions[inside_blocks][2] = { {5,5}, {20,8}, {10,15}, {3,14}, {49,30}, {10,10}, {10,8}, {45,25}, {12,21},{48,48} };
	for (int i = 0; i < inside_blocks; i++){

		objects[block_count+4+i] = Object(glm::vec3(positions[i][0], 1, positions[i][1]), glm::vec3(1.0f, 1.0f, 1.0f), mesh_cube, texture_cube);
	}

	// loading models and textures
	const int numModels = 4;
	Mesh mesh[numModels];
	Texture2D texture[numModels];

	mesh[0].loadOBJ("textures/crate.obj");
	mesh[1].loadOBJ("textures/floor.obj");
	mesh[2].loadOBJ("textures/robot.obj");
	mesh[3].loadOBJ("textures/woodcrate.obj");

	texture[0].loadTexture("textures/crate.jpg", true);
	texture[1].loadTexture("textures/tile_floor.jpg", true);
	texture[2].loadTexture("textures/robot_diffuse.jpg", true);	
	texture[3].loadTexture("textures/woodcrate_diffuse.jpg", true);

	double lastTime = glfwGetTime();
	float angle = 0.0;
	int counterRobot = 0;
	int robotDir = 0;

	// main loop
	while (!glfwWindowShouldClose(gWindow)) {
		showFPS(gWindow);
		
		double currentTime = glfwGetTime();
		double deltaTime = currentTime - lastTime;

		glfwPollEvents();
		update(deltaTime);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		
		glm::mat4 model(1.0), view(1.0), projection(1.0);


		view = fpsCamera.getViewMatrix();

		projection = glm::perspective(glm::radians(fpsCamera.getFOV()), (float)gWindowWidth / (float)gWindowHeight, 0.1f, 100.0f);

		glm::vec3 viewPos;
		viewPos.x = fpsCamera.getPosition().x;
		viewPos.y = fpsCamera.getPosition().y;
		viewPos.z = fpsCamera.getPosition().z;


		// moving the robot
		if (counterRobot < 500) {
			counterRobot += 1;
			if (robotDir == 0)
				objects[block_count + 2].position.z += 0.02f;
			else if (robotDir == 1)
				objects[block_count + 2].position.x += 0.02f;
			else if (robotDir == 2)
				objects[block_count + 2].position.z -= 0.02f;
			else
				objects[block_count + 2].position.x -= 0.02f;
		} else {
			counterRobot = 0;
			if (robotDir == 3)
				robotDir = 0;
			else
				robotDir += 1;
		}
			
		 
		// dafault color value
		glm::vec3 lightColor(1.0f, 0.80f, 0.0f);

		// point light
		glm::vec3 lightPointPos = fpsCamera.getPosition();
		lightPointPos.y -= 0.5f;

		angle += (float)deltaTime * 50.0f;
		lightPointPos.x = 3.0f * sinf(glm::radians(angle));
		lightPointPos.z = 14.0f + 10.0f * cosf(glm::radians(angle));
		lightPointPos.y += 4.8f;

		lighting.use();
		lighting.setUniform("view", view);
		lighting.setUniform("projection", projection);
		lighting.setUniform("viewPos", fpsCamera.getPosition());

		lighting.setUniform("pointLight.ambient", glm::vec3(0.7f, 0.7f, 0.7f));
		lighting.setUniform("pointLight.diffuse", lightColor);
		lighting.setUniform("pointLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		lighting.setUniform("pointLight.position", lightPointPos);
		
		lighting.setUniform("pointLight.constant", 1.0f);
		lighting.setUniform("pointLight.linear", 0.07f);
		lighting.setUniform("pointLight.exponent", 0.017f);
		
		// spot light
		glm::vec3 lightPos = fpsCamera.getPosition();
		lightPos.y -= 0.5f;

		lighting.setUniform("spotLight.ambient", glm::vec3(0.7f, 0.7f, 0.7f));
		lighting.setUniform("spotLight.diffuse", lightColor);
		lighting.setUniform("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		lighting.setUniform("spotLight.position", lightPos);
		lighting.setUniform("spotLight.direction", fpsCamera.getLook());

		lighting.setUniform("spotLight.constant", 1.0f);
		lighting.setUniform("spotLight.linear", 0.07f);
		lighting.setUniform("spotLight.exponent", 0.017f);
		lighting.setUniform("spotLight.cosInnerCone", glm::cos(glm::radians(15.0f)));
		lighting.setUniform("spotLight.cosOuterCone", glm::cos(glm::radians(30.0f)));
		lighting.setUniform("spotLight.on", gFlashlightOn);

		// directional / ambient
		lighting.setUniform("viewPos", fpsCamera.getPosition());
		lighting.setUniform("dirLight.direction", glm::vec3(0.0f, -0.9f, 0.17f));
		lighting.setUniform("dirLight.ambient", glm::vec3(0.7f, 0.7f, 0.7f));
		lighting.setUniform("dirLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
		lighting.setUniform("dirLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

		// enable the blend for the wall cubes
		glEnable(GL_BLEND);
		for (int i = 0; i < count; i++){

			model = glm::translate(glm::mat4(1.0), objects[i].position) * glm::scale(glm::mat4(1.0), objects[i].scale);

			lighting.setUniform("model", model);
			lighting.setUniform("material.ambient", glm::vec3(0.8f, 0.8f, 0.8f));
			lighting.setUniformSampler("material.diffuseMap", 0);
			lighting.setUniform("material.specular", glm::vec3(0.5f, 0.5f, 0.5f));
			lighting.setUniform("material.shininess", 32.0f);

			objects[i].texture.bind(0);	
			objects[i].mesh.draw();			// Render the OBJ mesh
			objects[i].texture.unbind(0);

			// disable the blend for other objects
			if (i > 120) {
				glDisable(GL_BLEND);
			}
		}
		glfwSwapBuffers(gWindow);

		lastTime = currentTime;
	}
	glfwTerminate();
	return 0;
}

void glfw_onKey(GLFWwindow* window, int key, int scancode, int action, int mode){

	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);

	if (key == GLFW_KEY_F1 && action == GLFW_PRESS){

		gWireframe = !gWireframe;
		if (gWireframe)
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
	if (key == GLFW_KEY_F && action == GLFW_PRESS) {
		gFlashlightOn = !gFlashlightOn;
	}
	if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
		fullScreen();
	}
}

// Switch between full screen and smaller window
void fullScreen() {
	fullscreen = !fullscreen;
	if (fullscreen) {
		GLFWmonitor* pMonitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* pVmode = glfwGetVideoMode(pMonitor);
		if (pVmode != NULL) {
			gWindowWidth = pVmode->width;
			gWindowHeight = pVmode->height;
			glfwSetWindowMonitor(gWindow, pMonitor, 0, 0, gWindowWidth, gWindowHeight, 0);
		}
	} else {
		gWindowWidth = 1024;
		gWindowHeight = 768;
		glfwSetWindowMonitor(gWindow, NULL, 100, 100, gWindowWidth, gWindowHeight, 0);
	}
}

// Mouse action
void glfw_onMouseScroll(GLFWwindow* window, double deltaX, double deltaY){

	double fov = fpsCamera.getFOV() + deltaY * ZOOM_SENSITIVITY;

	fov = glm::clamp(fov, 1.0, 120.0);

	fpsCamera.setFOV((float)fov);
}

// Collision detection
glm::vec3 checkCollisions(glm::vec3 offsetPos, glm::vec3 pos){

	glm::vec3 newOffsetPosition = glm::vec3(0,0,0);
	bool alright = true;
	bool fromRight;
	bool fromLeft;
	bool fromBottom;
	bool fromTop;
	// checking the movement on x coordinate
	for (int i = 0; i < count; i++){

		fromRight = offsetPos[0] < 0 && (((pos[0] + offsetPos[0] > objects[i].position[0]) && (objects[i].position[0] + 2 > pos[0] + offsetPos[0])) || ((pos[0] >= objects[i].position[0] + 2) && (pos[0] + 2 + offsetPos[0] <= objects[i].position[0])));
		fromLeft = offsetPos[0] > 0 && (((pos[0] + offsetPos[0] + 2 > objects[i].position[0]) && (pos[0] + offsetPos[0] + 2 < objects[i].position[0] + 2)) || ((pos[0] + 2 <= objects[i].position[0]) && (pos[0] + offsetPos[0] >= objects[i].position[0] + 2)));
		
		if (fromLeft || fromRight){

			if (fromLeft && pos[2] < objects[i].position[2] + 2 && pos[2] > objects[i].position[2] - 2){

				alright = false;
			}
			if (fromRight && pos[2] < objects[i].position[2] + 2 && pos[2] > objects[i].position[2] - 2){

				alright = false;
			}
		}
	}
	if (alright){

		newOffsetPosition[0] += offsetPos[0];
	}
	// checking the movement on z coordinate
	alright = true;
	for (int i = 0; i < count; i++){

		fromBottom = offsetPos[2] > 0 && (((pos[2] + 2 + offsetPos[2] > objects[i].position[2]) && (pos[2] + offsetPos[2] + 2 < objects[i].position[2] + 2)) || ((pos[2] + 2 <= objects[i].position[2]) && (pos[2] + 2 + offsetPos[2] >= objects[i].position[2] + 2)));
		fromTop = offsetPos[2] < 0 && (((pos[2] + offsetPos[2] < objects[i].position[2] + 2) && (pos[2] + offsetPos[2] > objects[i].position[2])) || ((pos[2] >= objects[i].position[2] + 2) && (pos[2] + offsetPos[2] <= objects[i].position[2] - 2)));
		
		if (fromBottom || fromTop){

			if (fromBottom && pos[0] < objects[i].position[0] + 2 && pos[0] > objects[i].position[0] - 2){

				alright = false;
			}
			if (fromTop && pos[0] < objects[i].position[0] + 2 && pos[0] > objects[i].position[0] - 2){

				alright = false;
			}
		}
	}
	if (alright){

		newOffsetPosition[2] += offsetPos[2];
	}
	return newOffsetPosition;
}

void update(double elapsedTime){
	
	// Camera orientation
	double mouseX, mouseY;

	// Get the current mouse cursor position delta
	glfwGetCursorPos(gWindow, &mouseX, &mouseY);

	// Rotate the camera the difference in mouse distance from the center screen.  Multiply this delta by a speed scaler
	fpsCamera.rotate((float)(gWindowWidth / 2.0 - mouseX) * MOUSE_SENSITIVITY, (float)(gWindowHeight / 2.0 - mouseY) * MOUSE_SENSITIVITY);

	// Clamp mouse cursor to center of screen
	glfwSetCursorPos(gWindow, gWindowWidth / 2.0, gWindowHeight / 2.0);

	// Moving the FPS camera
	// Forward/backward
	if (glfwGetKey(gWindow, GLFW_KEY_W) == GLFW_PRESS)
		fpsCamera.move(checkCollisions(MOVE_SPEED * (float)elapsedTime * fpsCamera.getLook(), fpsCamera.getPosition()));
	else if (glfwGetKey(gWindow, GLFW_KEY_S) == GLFW_PRESS)
		fpsCamera.move(checkCollisions(MOVE_SPEED * (float)elapsedTime * -fpsCamera.getLook(), fpsCamera.getPosition()));


	// Strafe left/right
	if (glfwGetKey(gWindow, GLFW_KEY_A) == GLFW_PRESS)
		fpsCamera.move(checkCollisions(MOVE_SPEED * (float)elapsedTime * -fpsCamera.getRight(),fpsCamera.getPosition()));
	else if (glfwGetKey(gWindow, GLFW_KEY_D) == GLFW_PRESS)
		fpsCamera.move(checkCollisions(MOVE_SPEED * (float)elapsedTime * fpsCamera.getRight(), fpsCamera.getPosition()));
}

void showFPS(GLFWwindow* window) {
	static double previousSeconds = 0.0;
	static int frameCount = 0;
	double elapsedSeconds;
	double currentSeconds = glfwGetTime();

	elapsedSeconds = currentSeconds - previousSeconds;

	if (elapsedSeconds > 0.25) {
		previousSeconds = currentSeconds;
		double fps = (double)frameCount / elapsedSeconds;
		double msPerFrame = 1000.0 / fps;

		std::ostringstream outs;
		outs.precision(3);
		outs << std::fixed
			<< APP_TITLE << "  "
			<< "FPS: " << fps << "  "
			<< "Frame Time: " << msPerFrame << "  (ms)";
		glfwSetWindowTitle(window, outs.str().c_str());

		frameCount = 0;
		frameCount++;
	}
}

void glfw_onFramebufferSize(GLFWwindow* window, int width, int height){

	gWindowWidth = width;
	gWindowHeight = height;
	glViewport(0, 0, gWindowWidth, gWindowHeight);
}

// init of all the libraries
bool initOpenGL() {
	if (!glfwInit()) {
		std::cerr << "GLFW initialization failed" << std::endl;
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	gWindow = glfwCreateWindow(gWindowWidth, gWindowHeight, APP_TITLE, NULL, NULL);

	if (gWindow == NULL) {
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
	}

	glfwMakeContextCurrent(gWindow);

	// Initialize GLEW
	glewExperimental = GL_TRUE;
	if (glewInit() != GLEW_OK){

		std::cerr << "Failed to initialize GLEW" << std::endl;
		return false;
	}

	// Set the required callback functions
	glfwSetKeyCallback(gWindow, glfw_onKey);
	glfwSetFramebufferSizeCallback(gWindow, glfw_onFramebufferSize);
	glfwSetScrollCallback(gWindow, glfw_onMouseScroll);

	// Hides and grabs cursor, unlimited movement
	glfwSetInputMode(gWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPos(gWindow, gWindowWidth / 2.0, gWindowHeight / 2.0);

	glClearColor(0.23f, 0.38f, 0.47f, 1.0f);

	// Define the viewport dimensions
	glViewport(0, 0, gWindowWidth, gWindowHeight);
	glEnable(GL_DEPTH_TEST);
	return true;
}



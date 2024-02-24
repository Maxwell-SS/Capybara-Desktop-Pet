// openGL
#include <glad/glad.h>

// glfw
#include <ApplicationServices/ApplicationServices.h>
#define GLFW_EXPOSE_NATIVE_COCOA
#define GLFW_EXPOSE_NATIVE_NSGL
#define GLFW_NATIVE_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <glm/ext.hpp>

// stb
// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// std
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <stdlib.h>
#include <time.h>
#include <chrono>
#include <filesystem>
#include <set>
#include <thread>
#include <sstream>
#include <mutex>
#include <future>
#include <mach/mach.h>
#include <random>

#include "GLare.hpp"

#ifdef __OBJC__
#define DEBUG 0
#include <Cocoa/Cocoa.h>
#endif

int width, height = 70;

float lastFrame, currentFrame;
float lastPrint = 0.0f;
float dt = 0.0f;
int frameCount = 0;

glm::mat4 projection;

float getRandomFloat(float lower, float upper) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> distrib(lower, upper);
	return distrib(gen);
}

bool getRandomBool() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 1);

	return dis(gen) == 1;
}

enum AnimationStates {
	Walk,
	Run,
	Idle,
	Sit,
	GetUp
};

struct Vertex {
	glm::vec3 position;
	glm::vec2 texCoord; 
};

struct Sprite {
	GLR::Texture sheet;
	AnimationStates state;
	int numberOfFrames;
	int currentFrameIndex;
	float frameDuration;

	Sprite() {}
	Sprite(GLR::Texture s, AnimationStates as, int n, int c, float f) : sheet(s), state(as), numberOfFrames(n), currentFrameIndex(c), frameDuration(f) {}
};

class Capybara {
public:
	Capybara() {}
	Capybara(glm::vec2 p, glm::vec2 s) : position(p), scale(s), velocity(glm::vec2(0.0f)), targetPosition(glm::vec2(0.0f)) {
		// loading textures
		std::string walkFile = std::string("/Users/max/Documents/c++/openGL/WonderEngine/res/sprites/Capybara_Walk.png");
		std::string runFile = std::string("/Users/max/Documents/c++/openGL/WonderEngine/res/sprites/Capybara_Run.png");
		std::string idleFile = std::string("/Users/max/Documents/c++/openGL/WonderEngine/res/sprites/Capybara_Idle.png");
		std::string sitFile = std::string("/Users/max/Documents/c++/openGL/WonderEngine/res/sprites/Capybara_Sit.png");

		walk = Sprite(GLR::Texture(walkFile), AnimationStates::Walk, 5, 0, 0.15f);
		run = Sprite(GLR::Texture(runFile), AnimationStates::Run, 5, 0, 0.1f);
		idle = Sprite(GLR::Texture(idleFile), AnimationStates::Idle, 5, 0, 0.2f);
		sit = Sprite(GLR::Texture(sitFile), AnimationStates::Sit, 5, 0, 0.1f);
		getup = Sprite(GLR::Texture(sitFile), AnimationStates::GetUp, 5, 0, 0.1f);

		// initial state
		currentSprite = &idle;
		state = AnimationStates::Idle;
		stateTimer = 0.0f;

		// animation variables
		elapsedTime = 0.0f;
		flipped = getRandomBool();

		// 2d square
		vertices = {
			{{ 0.5f,  0.5f, 0.0f}, {1.0f / (float)currentSprite->numberOfFrames, 1.0f}},
			{{ 0.5f, -0.5f, 0.0f}, {1.0f / (float)currentSprite->numberOfFrames, 0.0f}},
			{{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},
			{{-0.5f,  0.5f, 0.0f}, {0.0f, 1.0f}} 
		};
		indices = {
			0, 1, 3,
			1, 2, 3
		};

		// generate and bind VAO
		glGenVertexArrays(1, &vaoID);
		glBindVertexArray(vaoID);

		// generate and bind VBO
		glGenBuffers(1, &vboID);
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		// generate and bind EBO
		glGenBuffers(1, &eboID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		// set vertex attribute pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
		glEnableVertexAttribArray(1);

		// unbind VAO
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	void draw(GLR::Shader& shader) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(position, 0.0f));
		model = glm::scale(model, glm::vec3(scale, 1.0f));

		shader.bind();
		shader.setInt(std::string("ourTexture"), 0);
		shader.setMatrix4Float("u_model", glm::value_ptr(model));
		shader.setMatrix4Float("u_projection", glm::value_ptr(projection));

		currentSprite->sheet.bind(0);

		glBindVertexArray(vaoID);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		currentSprite->sheet.unbind();
		shader.unbind();
	}

	void updateTextureCoordinates() {
	   elapsedTime -= (float)currentSprite->frameDuration;

		float offset = currentSprite->currentFrameIndex * (1.0f / (float)currentSprite->numberOfFrames);
		if (flipped) {
			vertices[0].texCoord = {offset, 1.0f};  // Top left
			vertices[1].texCoord = {offset, 0.0f};  // Bottom left
			vertices[2].texCoord = {offset + (1.0f / (float)currentSprite->numberOfFrames), 0.0f};  // Bottom right
			vertices[3].texCoord = {offset + (1.0f / (float)currentSprite->numberOfFrames), 1.0f};  // Top right
		} else {
			vertices[0].texCoord = {offset + (1.0f / (float)currentSprite->numberOfFrames), 1.0f};  // Top right
			vertices[1].texCoord = {offset + (1.0f / (float)currentSprite->numberOfFrames), 0.0f};  // Bottom right
			vertices[2].texCoord = {offset, 0.0f};  // Bottom left
			vertices[3].texCoord = {offset, 1.0f};  // Top left
		}

		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void playAnimation(float deltaTime, bool looping = true) {
		elapsedTime += deltaTime;
		if (elapsedTime >= currentSprite->frameDuration) {
			if (looping) {
				currentSprite->currentFrameIndex = (currentSprite->currentFrameIndex + 1) % currentSprite->numberOfFrames;
			} else {
				currentSprite->currentFrameIndex = std::min(currentSprite->currentFrameIndex + 1, currentSprite->numberOfFrames - 1);
			}
			updateTextureCoordinates();
		}
	}

	void playAnimationReverse(float deltaTime, bool looping = true) {
		elapsedTime += deltaTime;
		if (elapsedTime >= currentSprite->frameDuration) {
			if (looping) {
				currentSprite->currentFrameIndex = (currentSprite->currentFrameIndex - 1 + currentSprite->numberOfFrames) % currentSprite->numberOfFrames;
			} else {
				currentSprite->currentFrameIndex = std::max(currentSprite->currentFrameIndex - 1, 0);
			}
			updateTextureCoordinates();
		}
	}

	void updateState(float deltaTime) {
		glm::vec2 direction;
		float nextState;

		stateTimer += deltaTime;

		switch (state) {
			case AnimationStates::Idle:
				nextState = getRandomFloat(0.0f, 1.0f);
				if (stateTimer > getRandomFloat(3.0f, 6.0f)) {
					if (nextState < 0.2f) { // 20% chance to stay Idle
						state = AnimationStates::Idle;
					} 
					else if (nextState < 0.6f) { // 40% chance to Walk
						state = AnimationStates::Walk;
					} 
					else if (nextState < 0.8f) { // 20% chance to Run
						state = AnimationStates::Run;
					} 
					else { // 20% chance to Sit
						state = AnimationStates::Sit;
					}
					targetPosition = glm::vec2(getRandomFloat(-5.0f, 5.0f), 0.0f);
					stateTimer = 0.0f;
				}
				break;

			case AnimationStates::Walk:
				direction = glm::normalize(targetPosition - position);
				position += direction * (1.0f * 0.5f) * deltaTime;
				if (direction.x > 0) {
					flipped = true;
				}
				if (direction.x < 0) {
					flipped = false;
				}
				if (glm::distance(position, targetPosition) < 0.1f) {
				    float nextState = getRandomFloat(0.0f, 1.0f);
				    if (nextState < 0.5f) {  // 50% chance to Idle
				        state = AnimationStates::Idle;
				    } else if (nextState < 0.7f) {  // 20% chance to Run
				    	targetPosition = glm::vec2(getRandomFloat(-5.0f, 5.0f), 0.0f);
				        state = AnimationStates::Run;
				    } else {  // 30% chance to Sit
				        state = AnimationStates::Sit;
				    }
				    stateTimer = 0.0f;
				}
				break;

			case AnimationStates::Run:
				direction = glm::normalize(targetPosition - position);
				position += direction * (1.0f * 1.0f) * deltaTime;
				if (direction.x > 0) {
					flipped = true;
				}
				if (direction.x < 0) {
					flipped = false;
				}
				if (glm::distance(position, targetPosition) < 0.1f) {
					state = AnimationStates::Walk;
					targetPosition = glm::vec2(getRandomFloat(-5.0f, 5.0f), 0.0f);
					stateTimer = 0.0f;
				}
				break;

			case AnimationStates::Sit:
				if (stateTimer > 3.0f) {
					state = GetUp;
					stateTimer = 0.0f;
				}
				break;

			case AnimationStates::GetUp:
				if (stateTimer > 0.5f) {
					state = Idle;
					stateTimer = 0.0f;
				}
				break;   
		}

		if (state == Walk) {
			currentSprite = &walk;
		}
		if (state == Run) {
			currentSprite = &run;
		}
		if (state == Idle) {
			currentSprite = &idle;
		}
		if (state == Sit) {
			currentSprite = &sit;
		}
		if (state == GetUp) {
			currentSprite = &sit;
		}

		if (state == GetUp) {
			playAnimationReverse(deltaTime, false);
		} 
		else {
			playAnimation(deltaTime, state != Sit);
		}
	}

private:
	Sprite walk;
	Sprite run;
	Sprite idle;
	Sprite sit;
	Sprite getup;

	Sprite* currentSprite;
	AnimationStates state;
	float stateTimer;

	float elapsedTime;
	bool flipped;

	glm::vec2 position;
	glm::vec2 scale;
	glm::vec2 velocity;
	glm::vec2 targetPosition;

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;

	GLuint vaoID, vboID, eboID;
};

int main(int argc, char* argv[]) {
	glfwInit();
	// window variables
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
	glfwWindowHint(GLFW_TRANSPARENT_FRAMEBUFFER, GLFW_TRUE);
	glfwWindowHint(GLFW_FLOATING, GLFW_TRUE);

	// using openGL version 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Mac os
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	const GLFWvidmode * mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
	std::cout << mode->width << " | " << mode->height << std::endl;
	// making glfw window
	width = mode->width;
	height = mode->height / 13;
	GLFWwindow* window = glfwCreateWindow(width, height, "GLare", NULL, NULL);
	if (window == NULL) {
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);

	// initializing glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	// setting position at bottom of screen
	glfwSetWindowPos(window, 0, 886);

	// enabling the depth buffer
	// glEnable(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);

	// Making png's see through
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLR::Shader shader(std::string("/Users/max/Documents/c++/openGL/WonderEngine/res/shader/test_vert.vert"), std::string("/Users/max/Documents/c++/openGL/WonderEngine/res/shader/test_frag.frag"));

	int numberOfCapybaras = 1;
	std::vector<Capybara> capies;
	for (int i = 0; i < numberOfCapybaras; ++i) {
		Capybara capy(glm::vec2(getRandomFloat(-5.0f, 5.0f), 0.0f), glm::vec2(0.5f, 0.5f));
		capies.push_back(capy);
	}

	// aspect ratio
	float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	float orthoWidth = 10.0f; // Adjust this value to scale your scene
	float orthoHeight = orthoWidth / aspectRatio;
	projection = glm::ortho(-orthoWidth / 2, orthoWidth / 2, -orthoHeight / 2, orthoHeight / 2, -1.0f, 1.0f);
	lastFrame = glfwGetTime();

	#ifdef __OBJC__
	NSWindow* cocoaWindow = glfwGetCocoaWindow(window);
	if (cocoaWindow)
	{
	    [cocoaWindow setLevel:NSFloatingWindowLevel];
	    [cocoaWindow setIgnoresMouseEvents:YES];
	}
	#endif

	while(!glfwWindowShouldClose(window)) {
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
			glfwSetWindowShouldClose(window, true);

		currentFrame = glfwGetTime();
		dt = currentFrame - lastFrame;
		lastFrame = currentFrame;

		frameCount++;
		if (currentFrame - lastPrint >= 1.0f) {
			std::cout << frameCount << std::endl;
			frameCount = 0;
			lastPrint = currentFrame;
		}

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// drawing
		for (int i = 0; i < capies.size(); ++i) {
			capies[i].updateState(dt);
			capies[i].draw(shader);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();    
	}
	
	glfwTerminate();
	return 0;
}
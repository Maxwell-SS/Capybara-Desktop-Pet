// #pragma once 

// struct Vertex {
// 	glm::vec3 position;
// 	glm::vec2 texCoord; 
// };

// class Sprite {
// public:
// 	Sprite() {}
// 	Sprite(GLR::Texture spriteSheet, int width, int height);

// 	void playAnimation(float deltaTime);
// 	void playAnimationReverse(float deltaTime);

// 	void draw(GLR::Shader& shader);

// private:
// 	std::vector<Vertex> vertices;
// 	std::vector<unsigned int> indices;

// 	GLR::VAO vao;
// 	GLR::VBO<Vertex> vbo;
// 	GLR::EBO ebo;

// 	GLR::Texture spriteSheet;

// 	int currentFrameIndex;
// 	int numFrames;
// 	float elapsedTime;
// 	float frameDuration;
// };

// enum CapybaraStates {
// 	Walk,
// 	Run,
// 	Idle,
// 	Sit
// };

// class Capybara {
// public:
// 	Capybara() {}
// 	Capybara(glm::vec2 position, glm::vec2 size, CapybaraStates state);

// 	void draw(float deltaTime);

// 	void run(glm::vec2 start, glm::vec2 end);
// 	void walk(glm::vec2 start, glm::vec2 end);
// 	void idle(float time);
// 	void sit(float time);

// private:
// 	glm::vec2 position;
// 	glm::vec2 size;
// 	float deltaTime;

// 	GLR::Shader shader;

// 	CapybaraStates state;

// 	Sprite capybaraWalk;
// 	Sprite capybaraRun;
// 	Sprite capybaraIdle;
// 	Sprite capybaraSit;
// };
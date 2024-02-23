void framebuffer_size_callback(GLFWwindow* window, int width, int height); 
void window_pos_callback(GLFWwindow* window, int xpos, int ypos);

int width, height = 70;

float lastFrame, currentFrame;
float lastPrint = 0.0f;
float dt = 0.0f;
int frameCount = 0;

glm::mat4 projection;

struct Vertex {
	glm::vec3 position;
	glm::vec2 texCoord; 
};

enum CapybaraStates {
	Walk,
	Run,
	Idle,
	Sit,
	GetUp
};

float getRandomFloat(float lower, float upper) {
	std::random_device rd; // Random device to seed the generator
	std::mt19937 gen(rd()); // Mersenne Twister pseudo-random generator
	std::uniform_real_distribution<> distrib(lower, upper); // Uniform distribution between lower and upper
	return distrib(gen); // Return the random number
}

bool getRandomBool() {
	std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, 1);

    return dis(gen) == 1;
}

class Sprite {
public:
	Sprite() {}
	Sprite(GLR::Texture& spriteSheet, glm::vec2 frameSize, float frameDuration)
		: spriteSheet(spriteSheet), frameSize(frameSize), currentFrameIndex(0), numFrames(spriteSheet.getWidth() / frameSize.x), elapsedTime(0.0f), frameDuration(frameDuration)  {

		position = glm::vec2(0.0f, 0.0f);
		scale = glm::vec2(0.5f, 0.5f);

		vertices = {
			{{ 0.5f,  0.5f, 0.0f}, {1.0f / (float)numFrames, 1.0f}},   // left
			{{ 0.5f, -0.5f, 0.0f}, {1.0f / (float)numFrames, 0.0f}},   // right
			{{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},   // top
			{{-0.5f,  0.5f, 0.0f}, {0.0f, 1.0f}}    // top
		};
		indices = {
			0, 1, 3,
			1, 2, 3
		};

		// Generate and bind VAO
		glGenVertexArrays(1, &vaoID);
		glBindVertexArray(vaoID);

		// Generate and bind VBO
		glGenBuffers(1, &vboID);
		glBindBuffer(GL_ARRAY_BUFFER, vboID);
		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);

		// Generate and bind EBO
		glGenBuffers(1, &eboID);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, eboID);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

		// Set vertex attribute pointers
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, position));
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, texCoord));
		glEnableVertexAttribArray(1);

		// Unbind VAO
		glBindVertexArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}

	~Sprite() {
		glDeleteVertexArrays(1, &vaoID);
		glDeleteBuffers(1, &vboID);
		glDeleteBuffers(1, &eboID);
	}

	void playAnimation(float deltaTime, bool looping = true) {
		elapsedTime += deltaTime;
		if (elapsedTime >= frameDuration) {
			if (looping) {
				currentFrameIndex = (currentFrameIndex + 1) % numFrames;
			} else {
				currentFrameIndex = std::min(currentFrameIndex + 1, numFrames - 1);
			}
			updateTextureCoordinates();
		}
	}

	void playAnimationReverse(float deltaTime, bool looping = true) {
		elapsedTime += deltaTime;
		if (elapsedTime >= frameDuration) {
			if (looping) {
				currentFrameIndex = (currentFrameIndex - 1 + numFrames) % numFrames;
			} else {
				currentFrameIndex = std::max(currentFrameIndex - 1, 0);
			}
			updateTextureCoordinates();
		}
	}

	void draw(GLR::Shader& shader) {
		glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(position, 0.0f));
		model = glm::scale(model, glm::vec3(scale, 1.0f));

		shader.bind();
		shader.setInt(std::string("ourTexture"), 0);
		shader.setMatrix4Float("u_model", glm::value_ptr(model));
		shader.setMatrix4Float("u_projection", glm::value_ptr(projection));

		spriteSheet.bind(0);

		glBindVertexArray(vaoID);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glBindVertexArray(0);

		spriteSheet.unbind();
		shader.unbind();
	}

	bool isAnimationFinished() { return currentFrameIndex == numFrames - 1 && elapsedTime >= frameDuration; }
	float getAnimationDuration() { return numFrames * frameDuration; }

	void setPosition(glm::vec2 newPosition) { position = newPosition; }
	void setScale(glm::vec2 newScale) { scale = newScale; }

	void setCurrentFrameIndex(int index) {
		currentFrameIndex = index;
		updateTextureCoordinates();
	}

	void flipSprite(bool flip) { 
		flipped = flip; 
		updateTextureCoordinates();
	}
	bool isSpriteFlipped() { return flipped; }

	int getNumFrames() { return numFrames; }

	glm::vec2 getPosition() { return position; }
	glm::vec2 getScale() { return scale; }

private:
	void updateTextureCoordinates() {
       elapsedTime -= frameDuration;

        float offset = currentFrameIndex * (1.0f / numFrames);
        if (flipped) {
            vertices[0].texCoord = {offset, 1.0f};  // Top left
            vertices[1].texCoord = {offset, 0.0f};  // Bottom left
            vertices[2].texCoord = {offset + (1.0f / numFrames), 0.0f};  // Bottom right
            vertices[3].texCoord = {offset + (1.0f / numFrames), 1.0f};  // Top right
        } else {
            vertices[0].texCoord = {offset + (1.0f / numFrames), 1.0f};  // Top right
            vertices[1].texCoord = {offset + (1.0f / numFrames), 0.0f};  // Bottom right
            vertices[2].texCoord = {offset, 0.0f};  // Bottom left
            vertices[3].texCoord = {offset, 1.0f};  // Top left
        }

        glBindBuffer(GL_ARRAY_BUFFER, vboID);
        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());
        glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
	GLR::Texture spriteSheet;

	int currentFrameIndex;
	int numFrames;
	float elapsedTime;
	float frameDuration;

	bool flipped;

	glm::vec2 position;
	glm::vec2 scale;

	glm::vec2 frameSize;
	GLuint vaoID, vboID, eboID;
};

struct Capybara {
	std::vector<Sprite> sprites;
	int currentSpriteIndex;
	glm::vec2 position;
	glm::vec2 velocity;
	glm::vec2 targetPosition;
	CapybaraStates state;
	float stateTimer;
	float speed;
	bool flipped;

	Capybara() : state(Idle), stateTimer(0.0f), speed(1.0f) {}

	void updateState(std::vector<Capybara>& capies, float deltaTime) {
		// Update state timer
		stateTimer += deltaTime;
		glm::vec2 direction;
		float nextState;

		// Check for state transitions
		switch (state) {
            case Idle:
                nextState = getRandomFloat(0.0f, 1.0f);
		        if (stateTimer > getRandomFloat(3.0f, 6.0f)) {
		            if (nextState < 0.2f) { // 20% chance to stay Idle
		                state = Idle;
		            } else if (nextState < 0.6f) { // 40% chance to Walk
		                state = Walk;
		            } else if (nextState < 0.8f) { // 20% chance to Run
		                state = Run;
		            } else { // 20% chance to Sit
		                state = Sit;
		            }
		            targetPosition = glm::vec2(getRandomFloat(-5.0f, 5.0f), 0.0f);
		            stateTimer = 0.0f;
		        }
		        break;
            case Walk:
                direction = glm::normalize(targetPosition - position);
                position += direction * (speed * 0.5f) * deltaTime;

                if (direction.x > 0) {
                	flipped = true;
                }
                if (direction.x < 0) {
                	flipped = false;
                }

                if (glm::distance(position, targetPosition) < 0.1f) {
                    state = static_cast<CapybaraStates>(int(getRandomFloat(0.0f, 4.0f)));
                    stateTimer = 0.0f;
                }
                break;
            case Run:
                direction = glm::normalize(targetPosition - position);
                position += direction * (speed * 1.0f) * deltaTime;

                if (direction.x > 0) {
                	flipped = true;
                }
                if (direction.x < 0) {
                	flipped = false;
                }

                if (glm::distance(position, targetPosition) < 0.1f) {
                    state = static_cast<CapybaraStates>(int(getRandomFloat(0.0f, 4.0f)));
                    stateTimer = 0.0f;
                }
                break;
            case Sit:
                if (stateTimer > 3.0f) {
                    state = GetUp;
                    stateTimer = 0.0f;
                }
                break;
            case GetUp:
                if (stateTimer > 0.5f) {
                    state = Idle;
                    stateTimer = 0.0f;
                }
                break;
        }

        // Update the sprite index and animation based on the state
        // currentSpriteIndex = static_cast<int>(state);
		if (state == Walk) {
			currentSpriteIndex = 0;
		}
		if (state == Run) {
			currentSpriteIndex = 1;
		}
		if (state == Idle) {
			currentSpriteIndex = 2;
		}
		if (state == Sit) {
			currentSpriteIndex = 3;
		}
		if (state == GetUp) {
			currentSpriteIndex = 3;
		}

        if (state == GetUp) {
            sprites[currentSpriteIndex].playAnimationReverse(deltaTime, false);
        } else {
            sprites[currentSpriteIndex].playAnimation(deltaTime, state != Sit);
        }
	}
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

	// if window changes size run the call back function
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);  

	glfwSetWindowPos(window, 0, 886);

	// enabling the depth buffer
	// glEnable(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);

	// Making png's see through
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	GLR::Shader shader(std::string("/Users/max/Documents/c++/openGL/WonderEngine/res/shader/test_vert.vert"), std::string("/Users/max/Documents/c++/openGL/WonderEngine/res/shader/test_frag.frag"));

	GLR::Texture texture_walk(std::string("/Users/max/Documents/c++/openGL/WonderEngine/res/sprites/Capybara_Walk.png"));
	GLR::Texture texture_run(std::string("/Users/max/Documents/c++/openGL/WonderEngine/res/sprites/Capybara_Run.png"));
	GLR::Texture texture_idle(std::string("/Users/max/Documents/c++/openGL/WonderEngine/res/sprites/Capybara_Idle.png"));
	GLR::Texture texture_sit(std::string("/Users/max/Documents/c++/openGL/WonderEngine/res/sprites/Capybara_Sit.png"));

	int numberOfCapies = 1;
	std::vector<Capybara> capies;
	for (int i = 0; i < numberOfCapies; ++i) {
		Capybara capy;

		Sprite sprite_walk(texture_walk, glm::vec2(32, 32), 0.1f);
		Sprite sprite_run(texture_run, glm::vec2(32, 32), 0.1f);
		Sprite sprite_idle(texture_idle, glm::vec2(32, 32), 0.2f);
		Sprite sprite_sit(texture_sit, glm::vec2(32, 32), 0.1f);

		capy.sprites.push_back(sprite_walk);
		capy.sprites.push_back(sprite_run);
		capy.sprites.push_back(sprite_idle);
		capy.sprites.push_back(sprite_sit);

		capy.currentSpriteIndex = 2;
		capy.state = Idle;
		capy.position = glm::vec2(getRandomFloat(-5.0f, 5.0f), 0.0f);
		capy.stateTimer = static_cast<int>(getRandomFloat(0.0f, 5.0f));
		capy.flipped = getRandomBool();

		capies.push_back(capy);
	}

	// when removed program crashes
	Sprite one(texture_walk, glm::vec2(32, 32), 0.1f);
	Sprite two(texture_run, glm::vec2(32, 32), 0.1f);
	Sprite three(texture_idle, glm::vec2(32, 32), 0.1f);
	Sprite four(texture_sit, glm::vec2(32, 32), 0.1f);

	// aspect ratio
	float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	float orthoWidth = 10.0f; // Adjust this value to scale your scene
	float orthoHeight = orthoWidth / aspectRatio;
	projection = glm::ortho(-orthoWidth / 2, orthoWidth / 2, -orthoHeight / 2, orthoHeight / 2, -1.0f, 1.0f);

	lastFrame = glfwGetTime();
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

		for (auto& capy : capies) {
			capy.updateState(capies, dt);
		}

		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// drawing
		{
			for (int i = 0; i < capies.size(); ++i) {
				if (capies[i].sprites[capies[i].currentSpriteIndex].isSpriteFlipped() != capies[i].flipped) {
					capies[i].sprites[capies[i].currentSpriteIndex].flipSprite(capies[i].flipped);
				}
				capies[i].sprites[capies[i].currentSpriteIndex].setPosition(capies[i].position);
				capies[i].sprites[capies[i].currentSpriteIndex].draw(shader);
			}
		}

		glfwSwapBuffers(window);
		glfwPollEvents();    
	}
	
	glfwTerminate();
	return 0;
}

// glfw callbacks
// callback to change viewport size when window size is changed
void framebuffer_size_callback(GLFWwindow* window, int newwidth, int newheight) {
	width = newwidth;
	height = newheight;
	glViewport(0, 0, width, height);

	float aspectRatio = static_cast<float>(width) / static_cast<float>(height);
	float orthoWidth = 10.0f; // Adjust this value to scale your scene
	float orthoHeight = orthoWidth / aspectRatio;
	projection = glm::ortho(-orthoWidth / 2, orthoWidth / 2, -orthoHeight / 2, orthoHeight / 2, -1.0f, 1.0f);
}





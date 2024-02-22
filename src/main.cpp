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
	Run
};

class Sprite {
public:
	Sprite() {}
    Sprite(GLR::Texture& spriteSheet, glm::vec2 frameSize)
        : spriteSheet(spriteSheet), frameSize(frameSize), currentFrameIndex(0), numFrames(spriteSheet.getWidth() / frameSize.x), elapsedTime(0.0f), frameDuration(0.1f)  {

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

    void update(float deltaTime) {
        elapsedTime += deltaTime;
        if (elapsedTime >= frameDuration) {
			currentFrameIndex = (currentFrameIndex + 1) % numFrames;
	        elapsedTime = 0.0f;

	        // Update texture coordinates based on currentFrameIndex
	        float offset = currentFrameIndex * (1.0f / numFrames);
	        vertices[0].texCoord = {offset + (1.0f / numFrames), 1.0f};  // Top right
	        vertices[1].texCoord = {offset + (1.0f / numFrames), 0.0f};  // Bottom right
	        vertices[2].texCoord = {offset, 0.0f};                       // Bottom left
	        vertices[3].texCoord = {offset, 1.0f};                       // Top left

	        // Update VBO with new texture coordinates
	        glBindBuffer(GL_ARRAY_BUFFER, vboID);
	        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());
	        glBindBuffer(GL_ARRAY_BUFFER, 0);
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

    void setPosition(glm::vec2 newPosition) { position = newPosition; }
    void setScale(glm::vec2 newScale) { scale = newScale; }

    glm::vec2 getPosition() { return position; }
    glm::vec2 getScale() { return scale; }

private:
	std::vector<Vertex> vertices;
	std::vector<unsigned int> indices;
    GLR::Texture spriteSheet;

	int currentFrameIndex;
	int numFrames;
	float elapsedTime;
	float frameDuration;

	glm::vec2 position;
	glm::vec2 scale;

    glm::vec2 frameSize;
    GLuint vaoID, vboID, eboID;
};

struct Capybara {
	std::vector<Sprite> sprites;
	Sprite currentSprite;
	glm::vec2 position;
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

	Sprite sprite_walk(texture_walk, glm::vec2(32, 32));
	Sprite sprite_run(texture_run, glm::vec2(32, 32));
	Sprite sprite_idle(texture_idle, glm::vec2(32, 32));
	Sprite sprite_sit(texture_sit, glm::vec2(32, 32));

	std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> distrib(-5.0, 5.0);

	int numberOfCapies = 5;
	std::vector<Capybara> capies;
	for (int i = 0; i < numberOfCapies; ++i) {
		Capybara capy;

		capy.sprites.push_back(sprite_walk);
		capy.sprites.push_back(sprite_run);
		capy.sprites.push_back(sprite_idle);
		capy.sprites.push_back(sprite_sit);

		capy.currentSprite = sprite_walk;

		capy.position = glm::vec2(distrib(gen), 0.0f);
		capy.currentSprite.setScale(glm::vec2(0.5f, 0.5f));

		capies.push_back(capy);
	}

	glm::vec2 one_position = glm::vec2(-0.2f, 0.0f);
	glm::vec2 two_position = glm::vec2(0.3f, 0.0f);

	Sprite one(texture_run, glm::vec2(32, 32));
	one.setPosition(one_position);
	one.setScale(glm::vec2(0.5f, 0.5f));

	Sprite two(texture_walk, glm::vec2(32, 32));
	two.setPosition(two_position);
	two.setScale(glm::vec2(0.5f, 0.5f));

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
    	one.update(currentFrame - lastFrame);
    	two.update(currentFrame - lastFrame);
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

		for (int i = 0; i < capies.size(); ++i) {
			capies[i].position.x -= dt * 0.3;

			if (capies[i].position.x < -5.2) {
				capies[i].position.x = 5.2;
			}
		}

		one_position.x -= dt * 0.6;
		one.setPosition(one_position);

		two_position.x -= dt * 0.3;
		two.setPosition(two_position);

		if (one_position.x < -5.2) {
			one_position.x = 5.2;
		}

		if (two_position.x < -5.2) {
			two_position.x = 5.2;
		}

		// drawing
		{
			one.draw(shader);
			two.draw(shader);

			for (int i = 0; i < capies.size(); ++i) {
				capies[i].currentSprite.setPosition(capies[i].position);;
				capies[i].currentSprite.draw(shader);
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





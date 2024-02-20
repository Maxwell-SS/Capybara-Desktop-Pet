void framebuffer_size_callback(GLFWwindow* window, int width, int height); 

int width = 600, height = 600;

float lastFrame, currentFrame;
float lastPrint = 0.0f;
int frameCount = 0;

struct Vertex {
	glm::vec3 position;
	glm::vec2 texCoord; 
};

class Sprite {
public:
    Sprite(GLR::Texture& spriteSheet, glm::vec2 frameSize)
        : spriteSheet(spriteSheet), frameSize(frameSize) {
        // Calculate the number of rows and columns in the sprite sheet
        rows = spriteSheet.getHeight() / static_cast<int>(frameSize.y);
        cols = spriteSheet.getWidth() / static_cast<int>(frameSize.x);

        currentFrameIndex = 0;
		numFrames = 5;
		elapsedTime = 0.0f;
		frameDuration = 0.1f;

    	vertices = {
		    {{ 0.5f,  0.5f, 0.0f}, {1.0f / (float)numFrames, 1.0f}},   // left
		    {{ 0.5f, -0.5f, 0.0f}, {1.0f / (float)numFrames, 0.0f}},   // right
		    {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},   // top
		    {{-0.5f,  0.5f, 0.0f}, {0.0f, 1.0f}}    // top
		};
		std::vector<unsigned int> indices = {
			0, 1, 3,
			1, 2, 3
		};

        // Initialize vertex data for a quad
        // std::vector<float> vertices = {
        //     // Positions         // Texture Coordinates
        //     0.5f,  0.5f, 0.0f,   1.0f, 1.0f, // Top Right
        //     0.5f, -0.5f, 0.0f,   1.0f, 0.0f, // Bottom Right
        //    -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, // Bottom Left
        //    -0.5f,  0.5f, 0.0f,   0.0f, 1.0f  // Top Left
        // };

        // std::vector<unsigned int> indices = {
        //     0, 1, 3, // First Triangle
        //     1, 2, 3  // Second Triangle
        // };

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

        // vao.linkAttribute(0, 3, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, position));
		// vao.linkAttribute(1, 2, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, texCoord));

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
        // if (elapsedTime >= 1.0f / animationSpeed) {
        //     currentFrame = (currentFrame + 1) % numFrames;
        //     elapsedTime = 0.0f;

        //     // Calculate the texture coordinates for the current frame
        //     int currentRow = currentFrame / cols;
        //     int currentCol = currentFrame % cols;

        //     float texLeft = static_cast<float>(currentCol) * frameSize.x / spriteSheet.getWidth();
        //     float texRight = static_cast<float>(currentCol + 1) * frameSize.x / spriteSheet.getWidth();
        //     float texTop = static_cast<float>(currentRow) * frameSize.y / spriteSheet.getHeight();
        //     float texBottom = static_cast<float>(currentRow + 1) * frameSize.y / spriteSheet.getHeight();

        //     std::vector<float> texCoords = {
        //         texRight, texTop,
        //         texRight, texBottom,
        //         texLeft, texBottom,
        //         texLeft, texTop
        //     };

        //     // Update the texture coordinates in the VBO
        //     glBindBuffer(GL_ARRAY_BUFFER, vboID);
        //     glBufferSubData(GL_ARRAY_BUFFER, 3 * sizeof(float), texCoords.size() * sizeof(float), texCoords.data());
        //     glBindBuffer(GL_ARRAY_BUFFER, 0);
        // }
        ////////////
        if (elapsedTime >= frameDuration) {
			currentFrameIndex = (currentFrameIndex + 1) % numFrames;
	        elapsedTime = 0.0f;

	        // Update texture coordinates based on currentFrameIndex
	        float offset = currentFrameIndex * (1.0f / numFrames);
	        vertices[0].texCoord = {offset + (1.0f / numFrames), 1.0f}; // Top right
	        vertices[1].texCoord = {offset + (1.0f / numFrames), 0.0f}; // Bottom right
	        vertices[2].texCoord = {offset, 0.0f};                       // Bottom left
	        vertices[3].texCoord = {offset, 1.0f};                       // Top left

	        // Update VBO with new texture coordinates
	        glBindBuffer(GL_ARRAY_BUFFER, vboID);
	        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());
	        glBindBuffer(GL_ARRAY_BUFFER, 0);
		}
    }

    void draw(GLR::Shader& shader, glm::vec2 scale, glm::vec2 position) {
    	glm::mat4 model = glm::mat4(1.0f);
		model = glm::translate(model, glm::vec3(position, 0.0f));
		model = glm::scale(model, glm::vec3(scale, 1.0f));

        shader.bind();
        shader.setInt(std::string("ourTexture"), 0);
        shader.setMatrix4Float("u_model", glm::value_ptr(model));

		spriteSheet.bind(0);

        glBindVertexArray(vaoID);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        spriteSheet.unbind();
        shader.unbind();
    }

private:
	int currentFrameIndex;
	int numFrames;
	float elapsedTime;
	float frameDuration;

	std::vector<Vertex> vertices;
    GLR::Texture spriteSheet;
    glm::vec2 frameSize;
    int rows, cols;
    GLuint vaoID, vboID, eboID;
};

int main(int argc, char* argv[]) {
	glfwInit();
	// using openGL version 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// Mac os
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
	
	// making glfw window
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

	// enabling the depth buffer
	// glEnable(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);

	// Making png's see through
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// Animation values
	float elapsedTime = 0.0f;
	int currentFrameIndex = 0;
	int numFrames = 5; // Number of frames in your animation
	float frameDuration = 0.1f; // Duration of each frame

	std::vector<Vertex> vertices = {
	    {{ 0.5f,  0.5f, 0.0f}, {1.0f / (float)numFrames, 1.0f}},   // left
	    {{ 0.5f, -0.5f, 0.0f}, {1.0f / (float)numFrames, 0.0f}},   // right
	    {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},   // top
	    {{-0.5f,  0.5f, 0.0f}, {0.0f, 1.0f}}    // top
	};
	std::vector<unsigned int> indices = {
		0, 1, 3,
		1, 2, 3
	};

	GLR::VAO vao;
	vao.bind();

	GLR::VBO vbo(vertices);
	GLR::EBO ebo(indices);

	vbo.bind();
	vao.linkAttribute(0, 3, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, position));
	vao.linkAttribute(1, 2, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, texCoord));
	vao.unbind();

	vbo.unbind();
	ebo.unbind();

	////////

	GLR::VAO avao;
	avao.bind();

	GLR::VBO avbo(vertices);
	GLR::EBO aebo(indices);

	avbo.bind();
	avao.linkAttribute(0, 3, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, position));
	avao.linkAttribute(1, 2, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, texCoord));
	avao.unbind();

	avbo.unbind();
	aebo.unbind();

	GLR::Shader shader(std::string("/Users/max/Documents/c++/openGL/WonderEngine/res/shader/test_vert.vert"), std::string("/Users/max/Documents/c++/openGL/WonderEngine/res/shader/test_frag.frag"));
	GLR::Texture texture(std::string("/Users/max/Documents/c++/openGL/WonderEngine/res/sprites/Capybara_Run.png"));
	GLR::Texture texture_run(std::string("/Users/max/Documents/c++/openGL/WonderEngine/res/sprites/Capybara_Run.png"));
	GLR::Texture texture_walk(std::string("/Users/max/Documents/c++/openGL/WonderEngine/res/sprites/Capybara_Walk.png"));

	glm::vec2 position = glm::vec2(-0.5f, 0.0f); // Example position
	glm::vec2 position_two = glm::vec2(0.5f, 0.0f); // Example position
	glm::vec2 scale = glm::vec2(0.5f, 0.5f); // Example scale (2x size)

	// Create a 4x4 identity matrix
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(position, 0.0f));
	model = glm::scale(model, glm::vec3(scale, 1.0f));

	glm::mat4 model_two = glm::mat4(1.0f);
	model_two = glm::translate(model_two, glm::vec3(position_two, 0.0f));
	model_two = glm::scale(model_two, glm::vec3(scale, 1.0f));

	Sprite one(texture_run, glm::vec2(32, 32));
	Sprite two(texture_walk, glm::vec2(32, 32));

	lastFrame = glfwGetTime();
	while(!glfwWindowShouldClose(window)) {
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

		currentFrame = glfwGetTime();
    	elapsedTime += currentFrame - lastFrame;
    	one.update(currentFrame - lastFrame);
    	two.update(currentFrame - lastFrame);
    	lastFrame = currentFrame;

		frameCount++;
		if (currentFrame - lastPrint >= 1.0f) {
			std::cout << frameCount << std::endl;
			frameCount = 0;
			lastPrint = currentFrame;
		}

		if (elapsedTime >= frameDuration) {
			currentFrameIndex = (currentFrameIndex + 1) % numFrames;
	        elapsedTime = 0.0f;

	        // Update texture coordinates based on currentFrameIndex
	        float offset = currentFrameIndex * (1.0f / numFrames);
	        vertices[0].texCoord = {offset + (1.0f / numFrames), 1.0f}; // Top right
	        vertices[1].texCoord = {offset + (1.0f / numFrames), 0.0f}; // Bottom right
	        vertices[2].texCoord = {offset, 0.0f};                       // Bottom left
	        vertices[3].texCoord = {offset, 1.0f};                       // Top left

	        // Update VBO with new texture coordinates
	        vbo.bind();
	        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());
	        vbo.unbind();

	        avbo.bind();
	        glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());
	        avbo.unbind();
		}

		glClearColor(1.0f, 0.5f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// drawing
		{
			one.draw(shader, glm::vec2(0.5f, 0.5f), glm::vec2(-0.1f, 0.0f));
			two.draw(shader, glm::vec2(0.5f, 0.5f), glm::vec2(0.1f, 0.0f));

			// shader.bind();
			// shader.setInt(std::string("ourTexture"), 0);
			// shader.setMatrix4Float("u_model", glm::value_ptr(model));
			// texture.bind(0);

			// vao.bind();
			// // glDrawArrays(GL_TRIANGLES, 0, 3);
			// glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
			// vao.unbind();

			// shader.unbind();
			// texture.unbind();

			// shader.bind();
			// shader.setInt(std::string("ourTexture"), 0);
			// shader.setMatrix4Float("u_model", glm::value_ptr(model_two));
			// texture.bind(0);

			// avao.bind();
			// // glDrawArrays(GL_TRIANGLES, 0, 3);
			// glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
			// avao.unbind();

			// shader.unbind();
			// texture.unbind();
		}

		glfwSwapBuffers(window);
		glfwPollEvents();    
	}

	vao.destroy();
	vbo.destroy();
	shader.destroy();
	texture.destroy();
	
	glfwTerminate();
	return 0;
}

// glfw callbacks
// callback to change viewport size when window size is changed
void framebuffer_size_callback(GLFWwindow* window, int newwidth, int newheight) {
	width = newwidth;
	height = newheight;
	glViewport(0, 0, width, height);
}

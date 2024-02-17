void framebuffer_size_callback(GLFWwindow* window, int width, int height); 

int width = 600, height = 600;

float lastFrame, currentFrame;
float lastPrint = 0.0f;
int frameCount = 0;

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
	glEnable(GL_DEPTH_TEST);

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

	GLR::Shader shader(std::string("/Users/max/Documents/c++/openGL/WonderEngine/res/shader/test_vert.vert"), std::string("/Users/max/Documents/c++/openGL/WonderEngine/res/shader/test_frag.frag"));
	GLR::Texture texture(std::string("/Users/max/Desktop/Capybara_Run.png"));

	lastFrame = glfwGetTime();
	while(!glfwWindowShouldClose(window)) {
		if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);

		currentFrame = glfwGetTime();
    	elapsedTime += currentFrame - lastFrame;
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
		}

		glClearColor(1.0f, 0.5f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// drawing
		{
			shader.bind();
			shader.setInt(std::string("ourTexture"), 0);
			texture.bind(0);

			vao.bind();
			// glDrawArrays(GL_TRIANGLES, 0, 3);
			glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
			vao.unbind();

			shader.unbind();
			texture.unbind();
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

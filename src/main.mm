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
// #include <glm/gtx/rotate_vector.hpp>
// #include <glm/gtx/vector_angle.hpp>
// #include <glm/gtx/string_cast.hpp>
// #include <glm/gtx/matrix_decompose.hpp>
// #include <glm/gtc/quaternion.hpp>
// #include <glm/gtx/euler_angles.hpp>
// #include <glm/ext.hpp>

// stb
// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// std
#include <iostream>
#include <string>
#include <vector>
#include <random>
#include <fstream>

#ifdef __OBJC__
#define DEBUG 0
#include <Cocoa/Cocoa.h>
#endif

int width, height;

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

class Debug {
public:
	static void checkOpenGLError() {
        GLenum error = glGetError();
        if (error != GL_NO_ERROR) {
            throw std::runtime_error("OpenGL error occurred: " + std::to_string(error));
        }
    }
};
class Shader {
public:
	Shader() {}
	Shader(const std::string& vertexFilePath, const std::string& fragmentFilePath) {
		compile(returnFileContents(vertexFilePath), returnFileContents(fragmentFilePath));
	}
	~Shader() { destroy(); }

	void setBool(const std::string& name, bool value) {
		glUniform1i(getUniformLocation(name), (int)value);
    	Debug::checkOpenGLError();
	}
	void setInt(const std::string& name, int value) {
		glUniform1i(getUniformLocation(name), (int)value);
    	Debug::checkOpenGLError();
	}
	void setFloat(const std::string& name, float value) {
		glUniform1f(getUniformLocation(name), value);
    	Debug::checkOpenGLError();
	}
	void setVector2Float(const std::string& name, const float* vec2) {
		glUniform2fv(getUniformLocation(name), 1, vec2);
    	Debug::checkOpenGLError();
	}
	void setVector3Float(const std::string& name, const float* vec3) {
		glUniform3fv(getUniformLocation(name), 1, vec3);
    	Debug::checkOpenGLError();
	}
	void setVector4Float(const std::string& name, const float* vec4) {
		glUniform4fv(getUniformLocation(name), 1, vec4);
    	Debug::checkOpenGLError();
	}
	void setMatrix4Float(const std::string& name, const float* mat4) {
		glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, mat4);
    	Debug::checkOpenGLError();
	}

	void bind() { glUseProgram(ID); }
	void unbind() { glUseProgram(0); }
	void destroy() { glDeleteProgram(ID); }

	GLuint getID() const { return ID; }

private:
	std::string returnFileContents(const std::string& filePath) {
		std::string contents; // contents for the file
		std::ifstream file(filePath, std::ios::in);

		// if unable to open file
		if (!file.is_open()) {
			std::cout << "error reading | " << filePath << " | Maybe wrong file name." << std::endl;
			return contents;
		}

		std::string line = "";
		while (!file.eof()) {
			std::getline(file, line);
			contents.append(line + "\n");
		}

		file.close();
		return contents;
	}
	void compile(const std::string& vertexContents, const std::string& fragmentContents) {
		const char* vertexSource = vertexContents.c_str();
		const char* fragmentSource = fragmentContents.c_str();

		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		// compiling vertex shader
		glShaderSource(vertexShader, 1, &vertexSource, NULL);
		glCompileShader(vertexShader);

		// compiling fragment shader
		glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
		glCompileShader(fragmentShader);

		// checking both shaders for errors
		compileErrorChecking(vertexShader);
		compileErrorChecking(fragmentShader);

		// linking shaders
		ID = glCreateProgram();
		glAttachShader(ID, vertexShader);
		glAttachShader(ID, fragmentShader);
		glLinkProgram(ID);

		// deleting shaders
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);
	}
	void compileErrorChecking(const GLuint& shaderID) {
		GLint compileStatus;
		glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileStatus);

		// if there is an error
		if (compileStatus != GL_TRUE) {
			GLint infoLogLenth;

			glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &infoLogLenth);
			GLchar* buffer = new GLchar[infoLogLenth];

			GLsizei bufferSize;
			glGetShaderInfoLog(shaderID, infoLogLenth, &bufferSize, buffer);

			std::cout << buffer << std::endl;

			delete [ ] buffer;
		}
	}
	GLint getUniformLocation(const std::string& name) {
        GLint location = glGetUniformLocation(ID, name.c_str());
        if (location == -1) {
            std::cerr << "Warning: Uniform '" << name << "' not found in shader program with ID: " << ID << std::endl;
        }
        return location;
    }

	GLuint ID;
};
class Texture {
public:
	Texture() : id(0), data(nullptr), width(0), height(0), nrChannels(0) {}
	Texture(const std::string& filename) : filename(filename), pixelType(GL_UNSIGNED_BYTE) {
		loadTexture();
		setFormat();
		createOpenGLTexture();

		// freeing memory
		stbi_image_free(data);
	}

	Texture(int width, int height, GLenum internalFormat, GLenum imageFormat, GLenum pixelType) : width(width), height(height), internalFormat(internalFormat), imageFormat(imageFormat), pixelType(pixelType)  {
		createOpenGLTexture();
	}

	void bind(int slot) {
		glActiveTexture(GL_TEXTURE0 + slot);
		glBindTexture(GL_TEXTURE_2D, id);
		Debug::checkOpenGLError();
	}
	void unbind() {
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	void destroy() {
		if (id) {
            glDeleteTextures(1, &id);
            id = 0;
        }
        if (data) {
            stbi_image_free(data);
            data = nullptr;
        }
	}

	GLuint getID() const { return id; }
	int getWidth() const { return width; }
	int getHeight() const { return height; }

private:
	void loadTexture() {
		stbi_set_flip_vertically_on_load(true); // flip the texture
		data = stbi_load(filename.c_str(), &width, &height, &nrChannels, 0);
		if (!data) {
			std::cout << "Failed to load image" << std::endl;
		}
	}
	void setFormat() {
		switch (nrChannels) {
            case 1: internalFormat = imageFormat = GL_RED; break;
            case 2: internalFormat = imageFormat = GL_RG; break;
            case 3: internalFormat = imageFormat = GL_RGB; break;
            case 4: internalFormat = imageFormat = GL_RGBA; break;
            default: throw std::runtime_error("Unsupported image format: " + filename);
        }
	}
	void createOpenGLTexture() {
	    	glGenTextures(1, &id);
	    	Debug::checkOpenGLError();
        glBindTexture(GL_TEXTURE_2D, id);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, imageFormat, pixelType, data);
        Debug::checkOpenGLError();

        glGenerateMipmap(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, 0);
	}

	std::string filename;
	unsigned int id;
	unsigned char *data;
	int width, height, nrChannels;
	GLenum internalFormat;
	GLenum imageFormat;
	GLenum pixelType;
};

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
	Texture sheet;
	AnimationStates state;
	int numberOfFrames;
	int currentFrameIndex;
	float frameDuration;

	Sprite() {}
	Sprite(Texture s, AnimationStates as, int n, int c, float f) : sheet(s), state(as), numberOfFrames(n), currentFrameIndex(c), frameDuration(f) {}
};

class Capybara {
public:
	Capybara() {}
	Capybara(glm::vec2 p, glm::vec2 s) : position(p), scale(s), velocity(glm::vec2(0.0f)), targetPosition(glm::vec2(0.0f)) {
		// loading textures
		std::string walkFile = std::string("res/sprites/Capybara_Walk.png");
		std::string runFile = std::string("res/sprites/Capybara_Run.png");
		std::string idleFile = std::string("res/sprites/Capybara_Idle.png");
		std::string sitFile = std::string("res/sprites/Capybara_Sit.png");

		walk = Sprite(Texture(walkFile), AnimationStates::Walk, 5, 0, 0.15f);
		run = Sprite(Texture(runFile), AnimationStates::Run, 5, 0, 0.1f);
		idle = Sprite(Texture(idleFile), AnimationStates::Idle, 5, 0, 0.2f);
		sit = Sprite(Texture(sitFile), AnimationStates::Sit, 5, 0, 0.1f);
		getup = Sprite(Texture(sitFile), AnimationStates::GetUp, 5, 0, 0.1f);

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

	void draw(Shader& shader) {
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
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

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

	// make it so the app wont show up in the dock or the force quit window
	[NSApplication sharedApplication];
    [NSApp setActivationPolicy:NSApplicationActivationPolicyAccessory];

	// setting position at bottom of screen
	glfwSetWindowPos(window, 0, mode->height - height);

	// enabling the depth buffer
	// glEnable(GL_DEPTH_TEST);
	glDisable(GL_DEPTH_TEST);

	// Making png's see through
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader shader(std::string("res/shader/test_vert.vert"), std::string("res/shader/test_frag.frag"));

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

	NSStatusItem *statusItem = [[NSStatusBar systemStatusBar] statusItemWithLength:NSVariableStatusItemLength];
    // Load the PNG image
    NSImage *iconImage = [[NSImage alloc] initWithContentsOfFile:@"res/icons/image.png"];
    // Set the image of the status item
    statusItem.button.image = iconImage;
    // Optional: Set the image scaling to ensure it fits well in the menu bar
    statusItem.button.imageScaling = NSImageScaleProportionallyDown;
    // Create a menu for the status item
    NSMenu *menu = [[NSMenu alloc] init];
    // Create a quit menu item
    NSMenuItem *quitItem = [[NSMenuItem alloc] initWithTitle:@"Quit" action:@selector(terminate:) keyEquivalent:@""];
    [menu addItem:quitItem];
    // Assign the menu to the status item
    statusItem.menu = menu;

	NSWindow* cocoaWindow = glfwGetCocoaWindow(window);
	if (cocoaWindow)
	{
		// if you want the window to appear infront of the dock
		// [cocoaWindow setLevel:NSStatusWindowLevel];
	    [cocoaWindow setLevel:NSFloatingWindowLevel];
	    [cocoaWindow setStyleMask:NSWindowStyleMaskBorderless];
	    [cocoaWindow setIgnoresMouseEvents:YES];
	    // [cocoaWindow setActivationPolicy:NSApplicationActivationPolicyAccessory];
	}

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
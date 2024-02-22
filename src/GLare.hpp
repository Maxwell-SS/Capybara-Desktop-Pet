#pragma once

// std
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>

// openGL
#include <glad/glad.h>

// stb
// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// Define "DEBUG" for error checking
#define DEBUG

namespace GLR {
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
	class Material {
	public:
		Material() {}
		Material(Shader shader) : shader(shader) {}

		void setTexture(const std::string& type, Texture& texture) {
	        textures[type] = &texture;
	    }

	    Texture& getTexture(const std::string& type) const {
	        auto it = textures.find(type);
	        if (it != textures.end()) {
	            return *(it->second);
	        }
	        throw std::runtime_error("Texture not found: " + type);
	    }

	    void bind() {
	        shader.bind();
	        int textureUnit = 0;
	        for (auto& pair : textures) {
	            pair.second->bind(textureUnit);
	            shader.setInt(pair.first, textureUnit);
	            textureUnit++;
	        }
	    }

	    void unbind() {
	        for (auto& pair : textures) {
	            pair.second->unbind();
	        }
	        shader.unbind();
	    }

	private:
		std::map<std::string, Texture*> textures;
		Shader shader;
	};

	template <typename T>
	class VBO {
	public:
		VBO() {}
		VBO(std::vector<T>& vertices) {
			glGenBuffers(1, &ID);
			glBindBuffer(GL_ARRAY_BUFFER, ID);
			glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(T), vertices.data(), GL_STATIC_DRAW);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			Debug::checkOpenGLError();
		}
		~VBO() { destroy(); }

		void bind() { glBindBuffer(GL_ARRAY_BUFFER, ID); }
		void unbind() { glBindBuffer(GL_ARRAY_BUFFER, 0); }
		void destroy() { glDeleteBuffers(1, &ID); }

	private:
		GLuint ID;
	};
	class VAO {
	public:
		VAO() { 
			glGenVertexArrays(1, &ID); 
			if (ID == 0) {
		        std::cerr << "Failed to create VAO" << std::endl;
		    }
			Debug::checkOpenGLError();
		}
		~VAO() { destroy(); }

		void linkAttribute(GLuint layout, GLuint numComponents, GLenum type, GLsizei stride, size_t offset, bool normalized = false) {
	        glVertexAttribPointer(layout, numComponents, type, normalized ? GL_TRUE : GL_FALSE, stride, (void*)offset);
			glEnableVertexAttribArray(layout);
		}

		void bind() { glBindVertexArray(ID); }
		void unbind() { glBindVertexArray(0); }
		void destroy() { glDeleteVertexArrays(1, &ID); }

	private:
		GLuint ID;
	};
	class EBO {
	public:
		EBO() {}
		EBO(std::vector<GLuint>& indices) {
			glGenBuffers(1, &ID);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);
			// unbind();
			Debug::checkOpenGLError();
		}
		~EBO() { destroy(); }

		void bind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ID); }
		void unbind() { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }
		void destroy() { glDeleteBuffers(1, &ID); }

	private:
		GLuint ID;
	};
	class FBO {
	public:
		FBO() { glGenFramebuffers(1, &ID); }
		~FBO() { destroy(); }

		void bind() { glBindFramebuffer(GL_FRAMEBUFFER, ID); }
		void unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }
		void destroy() { glDeleteFramebuffers(1, &ID); }

		void attachTexture(Texture& texture, GLenum attachment) {
			bind();
			glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_2D, texture.getID(), 0);
			checkFramebufferStatus();
			if (attachment == GL_DEPTH_ATTACHMENT) {
				glDrawBuffer(GL_NONE);
				glReadBuffer(GL_NONE);
			}
			unbind();
		}

	private:
		GLuint ID;

		void checkFramebufferStatus() const {
	        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
	            std::cerr << "Error: Framebuffer is not complete." << std::endl;
	            Debug::checkOpenGLError();
	        }
	    }
	};
	class UBO {
	public:
		UBO() { glGenBuffers(1, &ID); }
		~UBO() { destroy(); }

		void bind() { glBindBuffer(GL_UNIFORM_BUFFER, ID); }
		void unbind() { glBindBuffer(GL_UNIFORM_BUFFER, 0); }
		void destroy() { glDeleteBuffers(1, &ID); }

		void allocate(GLsizeiptr size, GLenum usage = GL_STATIC_DRAW) {
	        glBindBuffer(GL_UNIFORM_BUFFER, ID);
	        glBufferData(GL_UNIFORM_BUFFER, size, nullptr, usage);
	        glBindBuffer(GL_UNIFORM_BUFFER, 0);
	        Debug::checkOpenGLError();
	    }

	    void update(GLintptr offset, GLsizeiptr size, const void* data) {
	        bind();
	        glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
	        unbind();
	        Debug::checkOpenGLError();
	    }

	    void bindToIndex(GLuint index) {
	        glBindBufferBase(GL_UNIFORM_BUFFER, index, ID);
	    }

	private:
		GLuint ID;
	};
}
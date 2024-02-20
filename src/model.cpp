// #include "model.hpp"

// Sprite::Sprite(GLR::Texture spriteSheet, int width, int height) {
// 	// animation
// 	currentFrameIndex = 0;
// 	numFrames = spriteSheet.getWidth() / width; 
// 	elapsedTime = 0.0f;
// 	frameDuration = 0.1f;

// 	spriteSheet = spriteSheet;

// 	// square
// 	vertices = {
// 		{{ 0.5f,  0.5f, 0.0f}, {1.0f / (float)numFrames, 1.0f}},
// 	    {{ 0.5f, -0.5f, 0.0f}, {1.0f / (float)numFrames, 0.0f}},
// 	    {{-0.5f, -0.5f, 0.0f}, {0.0f, 0.0f}},
// 	    {{-0.5f,  0.5f, 0.0f}, {0.0f, 1.0f}}
// 	};
// 	indices = {
// 		0, 1, 3,
// 		1, 2, 3
// 	};

// 	// buffers
// 	GLR::VAO vao_ = GLR::VAO();
// 	vao_.bind();

// 	GLR::VBO vbo_ = GLR::VBO(vertices);
// 	GLR::EBO ebo_ = GLR::EBO(indices);

// 	vbo_.bind();
// 	vao_.linkAttribute(0, 3, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, position));
// 	vao_.linkAttribute(1, 2, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, texCoord));
// 	vao_.unbind();

// 	vbo_.unbind();
// 	ebo_.unbind();

// 	vao = vao_;
// 	vbo = vbo_;
// 	ebo = ebo_;
// }

// void Sprite::playAnimation(float deltaTime) {
// 	elapsedTime += deltaTime;
// 	if (elapsedTime >= frameDuration) {
// 		currentFrameIndex = (currentFrameIndex + 1) % numFrames;
//         elapsedTime = 0.0f;

//         // Update texture coordinates based on currentFrameIndex
//         float offset = currentFrameIndex * (1.0f / numFrames);
//         vertices[0].texCoord = {offset + (1.0f / numFrames), 1.0f};
//         vertices[1].texCoord = {offset + (1.0f / numFrames), 0.0f};
//         vertices[2].texCoord = {offset, 0.0f};
//         vertices[3].texCoord = {offset, 1.0f};

//         // Update VBO with new texture coordinates
//         vbo.bind();
//         glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());
//         vbo.unbind();
// 	}

// }
// void Sprite::playAnimationReverse(float deltaTime) {
// 	elapsedTime += deltaTime;
// 	if (elapsedTime >= frameDuration) {
// 	    // Decrement currentFrameIndex, and loop back to the last frame if it goes below 0
// 	    currentFrameIndex = (currentFrameIndex - 1 + numFrames) % numFrames;
// 	    elapsedTime = 0.0f;

// 	    // Update texture coordinates based on currentFrameIndex
// 	    float offset = currentFrameIndex * (1.0f / numFrames);
// 	    vertices[0].texCoord = {offset + (1.0f / numFrames), 1.0f};
// 	    vertices[1].texCoord = {offset + (1.0f / numFrames), 0.0f};
// 	    vertices[2].texCoord = {offset, 0.0f};                     
// 	    vertices[3].texCoord = {offset, 1.0f};                     

// 	    // Update VBO with new texture coordinates
// 	    vbo.bind();
// 	    glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(Vertex), vertices.data());
// 	    vbo.unbind();
// 	}
// }

// void Sprite::draw(GLR::Shader& shader) {
// 	shader.bind();
// 	shader.setInt(std::string("ourTexture"), 0);
// 	spriteSheet.bind(0);

// 	vao.bind();
// 	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
// 	vao.unbind();

// 	shader.unbind();
// 	spriteSheet.unbind();
// }


// Capybara::Capybara(glm::vec2 position, glm::vec2 size, CapybaraStates state) {
// 	position = position;
// 	size = size;
// 	state = state;

// 	shader = GLR::Shader(std::string("/Users/max/Documents/c++/openGL/WonderEngine/res/shader/test_vert.vert"), std::string("/Users/max/Documents/c++/openGL/WonderEngine/res/shader/test_frag.frag"));

// 	GLR::Texture walkSheet(std::string("/Users/max/Documents/c++/openGL/WonderEngine/res/sprites/Capybara_Walk.png"));
// 	GLR::Texture runSheet(std::string("/Users/max/Documents/c++/openGL/WonderEngine/res/sprites/Capybara_Run.png"));
// 	GLR::Texture idleSheet(std::string("/Users/max/Documents/c++/openGL/WonderEngine/res/sprites/Capybara_Idle.png"));
// 	GLR::Texture sitSheet(std::string("/Users/max/Documents/c++/openGL/WonderEngine/res/sprites/Capybara_Sit.png"));

// 	capybaraWalk = Sprite(walkSheet, 32, 32);
// 	capybaraRun = Sprite(runSheet, 32, 32);
// 	capybaraIdle = Sprite(idleSheet, 32, 32);
// 	capybaraSit = Sprite(sitSheet, 32, 32);
// }

// void Capybara::draw(float deltaTime) {
// 	deltaTime = deltaTime;

// 	if (state == CapybaraStates::Walk) {
// 		capybaraWalk.playAnimation(deltaTime);
// 		capybaraWalk.draw(shader);
// 	}
// }

// void Capybara::run(glm::vec2 start, glm::vec2 end) {
	
// }
// void Capybara::walk(glm::vec2 start, glm::vec2 end) {
	
// }
// void Capybara::idle(float time) {
	
// }
// void Capybara::sit(float time) {
	
// }







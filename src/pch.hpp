#pragma once

// openGL
#include <glad/glad.h>

// glfw
#include <GLFW/glfw3.h>

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

// assimp
// #include <assimp/Importer.hpp>
// #include <assimp/scene.h>
// #include <assimp/postprocess.h>
// #include <assimp/types.h>
// #include <assimp/matrix4x4.h>

// tinygltf
#include <tinygltf.h>

// stb
// #define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

// imgui
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// json
#include <json.hpp>

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

#include "GLare.hpp"
#include "model.hpp"

// only for mac os
// #include <sys/sysctl.h>
// #include <mach/mach.h>
cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 20)
project(OpenGL)

# delete folders
execute_process(COMMAND ${CMAKE_COMMAND} -E rm -rf res)

# copying shaders and resources file into build folder
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


include_directories(
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/libs/glad/include
	${PROJECT_SOURCE_DIR}/libs/glm/include
	${PROJECT_SOURCE_DIR}/libs/imgui/include
	${PROJECT_SOURCE_DIR}/libs/tinygltf/include
	${PROJECT_SOURCE_DIR}/libs/glfw/include
)

# main files
set(SOURCE
	${PROJECT_SOURCE_DIR}/src/main.mm
	${PROJECT_SOURCE_DIR}/src/GLare.hpp
	# ${PROJECT_SOURCE_DIR}/src/pch.hpp
)

# opengl
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
link_libraries(${OPENGL_INCLUDE_DIR})

# making main executable
add_executable(${PROJECT_NAME} ${SOURCE})

# glad
add_subdirectory(libs/glad EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} PRIVATE glad)

# glfw
add_subdirectory(libs/glfw EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} PRIVATE glfw)

# glm
add_subdirectory(libs/glm EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} PRIVATE glm)

# imgui
add_subdirectory(libs/imgui EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} PRIVATE imgui)

# tinygltf
add_subdirectory(libs/tinygltf EXCLUDE_FROM_ALL)
target_link_libraries(${PROJECT_NAME} PRIVATE tinygltf)

# cocoa
target_link_libraries(${PROJECT_NAME} PRIVATE "-framework Cocoa")
target_link_libraries(${PROJECT_NAME} PRIVATE "-framework Quartz")

# pch
# target_precompile_headers(${PROJECT_NAME} PUBLIC src/pch.hpp)

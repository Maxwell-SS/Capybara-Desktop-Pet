cmake_minimum_required(VERSION 3.2)
project(tinygltf)

# stb lib
add_library(tinygltf STATIC
	src/tinygltf.cpp
	include/json.hpp
	include/stb_image.h
	include/stb_image_write.h
	include/tinygltf.h)
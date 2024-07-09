cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 20)
project(Capybara)

option(BUILD_BUNDLE "Build as a macOS Application Bundle" OFF)

if(BUILD_BUNDLE)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Bundle)
endif()

include_directories(
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/libs/glad/include
	${PROJECT_SOURCE_DIR}/libs/glm/include
	${PROJECT_SOURCE_DIR}/libs/glfw/include
	${PROJECT_SOURCE_DIR}/libs/stb/include
)

set(SOURCE
	${PROJECT_SOURCE_DIR}/src/main.mm
)

# OpenGL
find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})
link_libraries(${OPENGL_LIBRARIES})

if(BUILD_BUNDLE)
	set(MACOSX_BUNDLE_ICON_FILE icon.icns)
	set(APP_ICON_MACOSX ${CMAKE_CURRENT_SOURCE_DIR}/res/icons/icon.icns)
	set_source_files_properties(${APP_ICON_MACOSX} PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

	add_executable(${PROJECT_NAME} MACOSX_BUNDLE ${SOURCE} ${APP_ICON_MACOSX})

	set_target_properties(${PROJECT_NAME} PROPERTIES
		MACOSX_BUNDLE_GUI_IDENTIFIER "com.example.opengl"
		MACOSX_BUNDLE_BUNDLE_NAME ${PROJECT_NAME}
		MACOSX_BUNDLE_BUNDLE_VERSION "1.0"
		MACOSX_BUNDLE_LONG_VERSION_STRING "1.0.0"
		MACOSX_BUNDLE_SHORT_VERSION_STRING "1.0"
		MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/info.plist
	)
else()
	add_executable(${PROJECT_NAME} ${SOURCE})
endif()

# libraries
add_subdirectory(libs/glad EXCLUDE_FROM_ALL)
add_subdirectory(libs/glfw EXCLUDE_FROM_ALL)
add_subdirectory(libs/glm EXCLUDE_FROM_ALL)
add_subdirectory(libs/stb EXCLUDE_FROM_ALL)

target_link_libraries(${PROJECT_NAME} PRIVATE glad glfw glm stb)

if(BUILD_BUNDLE)
	target_link_libraries(${PROJECT_NAME} PRIVATE "-framework Cocoa")
	target_link_libraries(${PROJECT_NAME} PRIVATE "-framework Quartz")

	# copy resources to the bundle
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy_directory
			${CMAKE_CURRENT_SOURCE_DIR}/res
			$<TARGET_FILE_DIR:${PROJECT_NAME}>/../Resources/res)
else()
	# copy resources to build folder
	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/res DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
endif()


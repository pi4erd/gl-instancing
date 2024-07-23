cmake_minimum_required(VERSION 3.22)

project(gl-instancing)

find_package(OpenGL REQUIRED)
find_package(glm REQUIRED)
find_package(GLEW REQUIRED)
find_package(glfw3 REQUIRED)
find_package(fmt REQUIRED)

# Custom built imgui library
find_package(imgui REQUIRED)

set(SOURCE_FILES src/main.cpp
    src/application.cpp src/material.cpp
    src/window.cpp src/texture.cpp
    src/shader.cpp src/mesh.cpp
    src/camera.cpp
)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME}
    PUBLIC OpenGL::GL
    PRIVATE glfw
    PRIVATE GLEW
    PRIVATE glm
    PRIVATE fmt
    PRIVATE imgui
)

add_custom_target(
    copy_shader_files
    ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/src/shaders ${CMAKE_BINARY_DIR}/shaders
    COMMENT "Copying shader directory."
)
add_custom_target(
    copy_assets
    ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/assets ${CMAKE_BINARY_DIR}/assets
    COMMENT "Copying assets."
)
add_dependencies(${PROJECT_NAME} copy_shader_files)
add_dependencies(${PROJECT_NAME} copy_assets)

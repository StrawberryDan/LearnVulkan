cmake_minimum_required(VERSION 3.20)
project(LearnVulkan)

set(CMAKE_CXX_STANDARD 23)

add_library(Vulkan INTERFACE)
target_link_libraries(Vulkan INTERFACE vulkan-1)

add_library(GLFW INTERFACE)
target_link_libraries(GLFW INTERFACE glfw3)

add_subdirectory("01 Instance Creation")
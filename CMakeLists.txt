cmake_minimum_required(VERSION 3.20)
project(LearnVulkan)

set(CMAKE_CXX_STANDARD 23)

add_library(Vulkan INTERFACE)
target_include_directories(Vulkan INTERFACE $ENV{VULKAN_ROOT}/Include)
target_link_directories(Vulkan INTERFACE $ENV{VULKAN_ROOT}/Lib)
target_link_libraries(Vulkan INTERFACE vulkan-1)

add_library(GLFW INTERFACE)
target_include_directories(GLFW INTERFACE $ENV{GLFW_ROOT}/include)
target_link_directories(GLFW INTERFACE $ENV{GLFW_ROOT}/lib)
target_link_libraries(GLFW INTERFACE glfw3)

add_subdirectory("01 Instance Creation")
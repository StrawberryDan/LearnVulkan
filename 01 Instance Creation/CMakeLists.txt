add_executable(01_Instance_Creation main.cpp)
target_include_directories(01_Instance_Creation PRIVATE $ENV{GLFW_ROOT}/include $ENV{VULKAN_ROOT}/Include)
target_link_directories(01_Instance_Creation PRIVATE $ENV{GLFW_ROOT}/lib $ENV{VULKAN_ROOT}/Lib)
target_link_libraries(01_Instance_Creation PRIVATE glfw3 vulkan-1)
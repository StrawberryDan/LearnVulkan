#include <iostream>
#include <vector>
#include <sstream>
#include <bitset>
#include <algorithm>

#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

/**
 * Loads a vulkan function from an instance. Creates the variable in which to store the function pointer in-place.
 * @param INSTANCE The vulkan instance to load the function from (may be NULL/nullptr for certain function names).
 * @param NAME     The name of the function to load and the name of the variable which will keep the function pointer.
 */
#define LOAD_VK_FN(INSTANCE, NAME) glfwGetInstanceProcAddress(instance, #NAME);

/**
 * Gets the string version of version.
 * @param version The vulkan version number.
 * @return A string representation of vulkan.
 */
std::string vulkan_api_version_to_string(uint32_t version) {
    std::stringstream stream;
    stream << VK_VERSION_MAJOR(version) << "."
           << VK_VERSION_MINOR(version) << "."
           << VK_VERSION_PATCH(version) <<
           " (Variant: " << VK_API_VERSION_VARIANT(version) << ")";
    return stream.str();
}

/**
 * Gets the string of a physical device type.
 * @param type The vulkan physical device type.
 * @return The string version of type.
 */
std::string vulkan_physical_device_type_to_string(VkPhysicalDeviceType type) {
    switch (type) {
        case VK_PHYSICAL_DEVICE_TYPE_OTHER:
            return "Other";
        case VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU:
            return "Integrated GPU";
        case VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU:
            return "Discrete GPU";
        case VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU:
            return "Virtual GPU";
        case VK_PHYSICAL_DEVICE_TYPE_CPU:
            return "CPU";
        default:
            std::cerr << "Invalid Physical Device Type" << std::endl;
            std::exit(-1);
    }
}

/**
 * Initialises vulkan instance object.
 * If not already specified, adds the necessary GLFW extensions for surface display.
 * @param layers The names of the layers to load in the instance.
 * @param extensions The names of the extensions to load in the instance.
 * @return An initialised vulkan instance.
 */
VkInstance initialise_vulkan(std::vector<const char *> layers, std::vector<const char *> extensions) {
    VkInstance instance = VK_NULL_HANDLE;
    VkInstanceCreateInfo instance_create_info;
    VkApplicationInfo instance_application_info;

    uint32_t glfw_required_extension_count = 0;
    const char **glfw_required_extensions = glfwGetRequiredInstanceExtensions(&glfw_required_extension_count);

    for (int i = 0; i < glfw_required_extension_count; i++) {
        if (std::count(extensions.begin(), extensions.end(), glfw_required_extensions[i]) == 0) {
            extensions.push_back(glfw_required_extensions[i]);
        }
    }

#ifndef NDEBUG
    layers.push_back("VK_LAYER_KHRONOS_validation");
#endif

    LOAD_VK_FN(nullptr, vkCreateInstance)
    LOAD_VK_FN(nullptr, vkDestroyInstance);

    instance_create_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_create_info.pNext = nullptr;
    instance_create_info.flags = 0;
    instance_create_info.pApplicationInfo = &instance_application_info;
    instance_create_info.enabledLayerCount = layers.size();
    instance_create_info.ppEnabledLayerNames = layers.data();
    instance_create_info.enabledExtensionCount = extensions.size();
    instance_create_info.ppEnabledExtensionNames = extensions.data();
    instance_application_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    instance_application_info.pNext = nullptr;
    instance_application_info.pApplicationName = "LearnVulkan";
    instance_application_info.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    instance_application_info.pEngineName = "LearnVulkanEngine";
    instance_application_info.engineVersion = VK_MAKE_VERSION(0, 1, 0);
    instance_application_info.apiVersion = VK_API_VERSION_1_2;

    if (vkCreateInstance(&instance_create_info, nullptr, &instance) != VK_SUCCESS) {
        throw std::runtime_error("Unable to create vulkan instance");
    }

    return instance;
}

/**
 * Loads needed vulkan functions.
 * @param instance The instance from which to load vulkan functions.
 */
void load_vulkan_functions(VkInstance instance) {
    LOAD_VK_FN(instance, vkEnumerateInstanceVersion)
    LOAD_VK_FN(instance, vkEnumeratePhysicalDevices)
    LOAD_VK_FN(instance, vkGetPhysicalDeviceProperties)
    LOAD_VK_FN(instance, vkGetPhysicalDeviceQueueFamilyProperties)
}

/**
 * Enumerates and vectorises all vulkan physical devices.
 * @param instance The vulkan instance with which the devices are associated.
 * @return A vector array of vulkan devices.
 */
std::vector<VkPhysicalDevice> get_physical_devices(VkInstance instance) {
    uint32_t device_count = 0;
    if (vkEnumeratePhysicalDevices(instance, &device_count, nullptr) != VK_SUCCESS) {
        throw std::runtime_error("Unable to enumerate physical vulkan devices");
    }

    std::vector<VkPhysicalDevice> devices = {};
    devices.resize(device_count);
    if (vkEnumeratePhysicalDevices(instance, &device_count, devices.data()) != VK_SUCCESS) {
        throw std::runtime_error("Unable to enumerate physical vulkan devices");
    }

    return devices;
}

/**
 * Returns the properties of an array of vulkan physical devices.
 * @param physical_devices The devices of which properties should be queried.
 * @return A array of physical device properties. In the same order as the physical device array given.
 */
std::vector<VkPhysicalDeviceProperties>
get_physical_device_properties(const std::vector<VkPhysicalDevice> &physical_devices) {
    std::vector<VkPhysicalDeviceProperties> physical_device_properties;
    for (auto &physical_device: physical_devices) {
        auto &properties = physical_device_properties.emplace_back();
        vkGetPhysicalDeviceProperties(physical_device, &properties);
    }

    return physical_device_properties;
}

std::vector<std::vector<VkQueueFamilyProperties>>
get_physical_device_queue_family_properties(const std::vector<VkPhysicalDevice> &physical_devices) {
    std::vector<std::vector<VkQueueFamilyProperties>> physical_device_queue_family_properties;
    for (int i = 0; i < physical_devices.size(); i++) {
        // Get queue family count
        uint32_t queue_family_count = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physical_devices[i], &queue_family_count, nullptr);
        // Read queue family properties
        std::vector<VkQueueFamilyProperties> queue_properties;
        queue_properties.resize(queue_family_count);
        vkGetPhysicalDeviceQueueFamilyProperties(physical_devices[i], &queue_family_count, queue_properties.data());
        physical_device_queue_family_properties.push_back(queue_properties);
    }

    return physical_device_queue_family_properties;
}

/**
 * Return a human-readable string describing an instance of a VkQueueFamilyProperties object.
 * @param index The queue index of the queue.
 * @param properties The properties obect to be printed.
 * @return The string describing the object.
 */
std::string queue_family_properties_to_string(unsigned int index, VkQueueFamilyProperties properties) {
    std::stringstream str;
    str << "    [Queue " << index << "]" << std::endl
        << "        Queue Count: " << properties.queueCount << std::endl
        << "        Queue Capabilities: " << std::endl
        << "            Graphics:       "
        << ((properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) ? "True" : "False") << std::endl
        << "            Compute:        "
        << ((properties.queueFlags & VK_QUEUE_COMPUTE_BIT) ? "True" : "False") << std::endl
        << "            Transfer:       "
        << ((properties.queueFlags & VK_QUEUE_TRANSFER_BIT) ? "True" : "False") << std::endl
        << "            Sparse Binding: "
        << ((properties.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) ? "True" : "False") << std::endl
        << "            Protected:      "
        << ((properties.queueFlags & VK_QUEUE_PROTECTED_BIT) ? "True" : "False") << std::endl
        << std::endl;
    return str.str();
}

int main() {
    if (glfwInit() != GLFW_TRUE) {
        std::cerr << "Failed to initialise GLFW. Aborting with code -1" << std::endl;
        throw std::runtime_error("Unable to load GLFW3");
    }

    if (!glfwVulkanSupported()) {
        std::cerr << "Vulkan is not supported. Aborting with code: -1" << std::endl;
        glfwTerminate();
        throw std::runtime_error("Vulkan is not supported on this host");
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    auto instance = initialise_vulkan({}, {});
    load_vulkan_functions(instance);

    uint32_t instance_version = 0;
    vkEnumerateInstanceVersion(&instance_version);
    std::cout << "Vulkan API Version found: " << vulkan_api_version_to_string(instance_version) << std::endl;
    std::cout << std::endl;

    auto physical_devices = get_physical_devices(instance);
    std::cout << "Found " << physical_devices.size() << " physical vulkan devices" << std::endl;

    std::cout << std::endl;
    auto physical_device_properties = get_physical_device_properties(physical_devices);
    for (auto &properties: physical_device_properties) {
        std::cout << "Found Device: " << properties.deviceName << std::endl
                  << "    Type:                    " << vulkan_physical_device_type_to_string(properties.deviceType)
                  << std::endl
                  << "    Supports Vulkan Version: " << vulkan_api_version_to_string(properties.apiVersion)
                  << std::endl;
    }

    std::cout << std::endl;
    auto physical_device_queue_family_properties = get_physical_device_queue_family_properties(physical_devices);
    for (int device_idx = 0; device_idx < physical_devices.size(); device_idx++) {
        std::cout << "Found " << physical_device_queue_family_properties[device_idx].size()
                  << " queue families for device "
                  << physical_device_properties[device_idx].deviceName << std::endl;

        auto queue_family_count = physical_device_queue_family_properties[device_idx].size();
        for (int queue_family_idx = 0; queue_family_idx < queue_family_count; queue_family_idx++) {
            auto &queue_family = physical_device_queue_family_properties[device_idx][queue_family_idx];
            std::cout << queue_family_properties_to_string(queue_family_idx, queue_family);
        }
    }

    vkDestroyInstance(instance, nullptr);

    glfwTerminate();

    return 0;
}


#include<core/VulkanSurface.h>

me::VulkanSurface::VulkanSurface(vk::raii::Instance& instance, GLFWwindow* window)
{
	CreateSurface(instance, window);
}

me::VulkanSurface::~VulkanSurface()
{

}

void me::VulkanSurface::CreateSurface(vk::raii::Instance& instance, GLFWwindow* window)
{
	VkSurfaceKHR surface;
	if (glfwCreateWindowSurface(static_cast<VkInstance>(*instance), window, nullptr, &surface) != VK_SUCCESS) {
		throw std::runtime_error("failed to create window surface");
	}
	info.surface = vk::raii::SurfaceKHR(instance, surface);
	if (info.surface == nullptr) {
		std::cerr << "Vulkan surface create failed!" << std::endl;
		return;
	}
	else {
		std::clog << "Vulkan surface create success!" << std::endl;
	}
}
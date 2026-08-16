//Vulkan渲染界面

#pragma once
#include<iostream>
#include<vulkan/vulkan_raii.hpp>

#define GLFW_INCLUDE_VULKAN
#include<GLFW/glfw3.h>

namespace me {

	//Vulkan渲染界面树形结构体
	struct VulkanSurfaceInfo {
		vk::raii::SurfaceKHR surface = nullptr; //Vulkan渲染界面
	};

	//Vulkan渲染界面类
	class VulkanSurface
	{
	public:
		VulkanSurfaceInfo info;

	public:
		VulkanSurface(vk::raii::Instance& instance, GLFWwindow* window);
		~VulkanSurface();
	private:
		//创建渲染界面
		void CreateSurface(vk::raii::Instance& instance,GLFWwindow* window);
	};
}
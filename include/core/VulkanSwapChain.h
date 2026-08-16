//Vulkan交换链类

#pragma once
#include<iostream>
#include<algorithm>
#include<assert.h>
#include<GLFW/glfw3.h>
#include<vulkan/vulkan_raii.hpp>

namespace me {

	//Vulkan交换链属性结构体
	struct VulkanSwapChainInfo {
		vk::raii::SwapchainKHR swapChain = nullptr; //Vulkan交换链
		vk::SurfaceFormatKHR swapChainFormat;       //交换链的输出格式
		vk::Extent2D         swapChainExtent;       //交换链的范围大小
		std::vector<vk::Image>swapChainImages;      //交换链图像
		std::vector<vk::raii::ImageView> swapChainImageViews;//图像视图，注意：这里的交换链图像的图像视图是vk::raii::ImageView格式，否则后续绘制图像时会出现格式不匹配问题（2026.03.16）
		uint32_t maxImage = 3u;                     //最大图像数量
		bool framebufferResized = false;            //
		uint32_t frameIndex = 0; //帧并行录制命令时，具体帧的索引
	};

	class VulkanSwapChain
	{
	public:
		VulkanSwapChainInfo info;
	
	public:
		VulkanSwapChain(
			vk::raii::PhysicalDevice& physicalDevice, 
			vk::raii::Device& logicalDevice, 
			vk::raii::SurfaceKHR& surface,
			GLFWwindow* window);
		~VulkanSwapChain();

	private:
		//创建交换链
		void CreateSwapChain(
			vk::raii::PhysicalDevice& physicalDevice, 
			vk::raii::Device& logicalDevice, 
			vk::raii::SurfaceKHR& surface,
			GLFWwindow* window);

		//创建图像视图
		void CreateImageView(vk::raii::Device& logicalDevice);

		//选择交换链的图形数量
		uint32_t ChooseSwapChainImageCount(vk::SurfaceCapabilitiesKHR surfaceCapabilities);

		//选择交换链渲染界面格式
		vk::SurfaceFormatKHR ChooseSwapChainSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats);

		//设置交换链交换的范围大小
		vk::Extent2D ChooseSwapChainExtent(vk::SurfaceCapabilitiesKHR& surfaceCapabilities, GLFWwindow* window);
		
		//设置交换链的图像呈现方式
		vk::PresentModeKHR ChooseSwapChainPresent(const std::vector<vk::PresentModeKHR>& presents);
	
		//设置图像的贴图格式
		vk::ComponentMapping ChooseImageViewMapping();

		//设置图像子通道格式
		vk::ImageSubresourceRange ChooseImageViewSubresourceRange();
	};
}
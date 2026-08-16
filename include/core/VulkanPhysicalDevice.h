//Vulkan物理设备类

#pragma once
#include<iostream>
#include<stdexcept>
#include<vulkan/vulkan_raii.hpp>

namespace me {
	
	//Vulkan物理设备创建类
	struct VulkanPhysicalDeviceInfo {
		vk::raii::PhysicalDevice physicalDevice = nullptr; //Vulkan物理设备
		std::vector<const char*> requiredExtensions = {
			vk::KHRSwapchainExtensionName,              //交换链扩展名
			vk::KHRSpirv14ExtensionName,                //SPIR-V着色器扩展名
			vk::KHRSynchronization2ExtensionName,       //同步原语扩展名
			vk::KHRCreateRenderpass2ExtensionName,      //渲染通道扩展名
			vk::KHRShaderDrawParametersExtensionName,   //着色器绘制参数修改扩展名
			vk::KHRDynamicRenderingExtensionName        //动态渲染扩展名
		};
	};

	//Vulkan物理设备属性信息结构体
	class VulkanPhysicalDevice
	{
	public:
		VulkanPhysicalDeviceInfo info;

	public:
		VulkanPhysicalDevice(vk::raii::Instance& instance);
		~VulkanPhysicalDevice();

	private:
		//选取合适的物理设备
		void pickPhysicalDevice(vk::raii::Instance& instance);

		//检查版本支持性
		bool CheckVersionSupported(vk::raii::PhysicalDevice& device);

		//检查队列支持性
		bool CheckQueueSupported(vk::raii::PhysicalDevice& device);

		//检查扩展支持性
		bool CheckExtensionSupported(vk::raii::PhysicalDevice& device);

		//检查设备具备的特性
		bool CheckDeviceFeaturesSupported(vk::raii::PhysicalDevice& device);
	};
}


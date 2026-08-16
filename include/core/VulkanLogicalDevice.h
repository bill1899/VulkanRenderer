//Vulkan逻辑设备类

#pragma once
#include<iostream>
#include<memory>
#include<vulkan/vulkan_raii.hpp>


namespace me{

	class VulkanQueueFamily;

	//Vulkan逻辑设备的属性结构体
	struct VulkanLogicalDeviceInfo {
		vk::raii::Device logicalDevice = nullptr; //Vulkan逻辑设备
		vk::StructureChain<
			vk::PhysicalDeviceFeatures2,
			vk::PhysicalDeviceVulkan13Features,
			vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT
		> enableFeatures;                         //Vulkan逻辑设备启用的特性
		std::unique_ptr<VulkanQueueFamily> queue = nullptr;
	};

	class VulkanLogicalDevice
	{
	public:
		VulkanLogicalDeviceInfo info;

	public:
		VulkanLogicalDevice(vk::raii::PhysicalDevice& physicalDevcie, std::vector<const char*> requiredExtensions);
		~VulkanLogicalDevice();

	private:
		//创建逻辑设备
		void CreateLogicalDevice(vk::raii::PhysicalDevice& physicalDevcie, std::vector<const char*> requiredExtensions);

		//创建队列家族
		void CreateQueueFamily(vk::raii::PhysicalDevice& physicalDevice);

		//配置逻辑设备的结构体
		void setupLogicalDevice(vk::raii::PhysicalDevice& pgysicalDevice, std::vector<const char*> requiredExtensions);
		
		//启用特性
		void enableFeatures();
	};
}

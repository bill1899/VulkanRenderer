//Vulkan队列家族

#pragma once
#include<iostream>
#include<vulkan/vulkan_raii.hpp>

namespace me {

	//Vulkan队列家族属性结构体
	struct VulkanQueueFamilyInfo {
		vk::raii::Queue graphicsQueue = nullptr; //图形队列
		vk::raii::Queue transferQueue = nullptr; //传输队列
		vk::raii::Queue computeQueue = nullptr;  //计算队列
		uint32_t graphicsQueueIndex   = ~0;      //图形队列索引
		uint32_t transferQueueIndex   = ~0;      //传输队列索引
		uint32_t computeQueueIndex    = ~0;      //计算队列索引
		uint32_t queueCount           = 3;       //队列族中队列的数量 
		std::vector<vk::DeviceQueueCreateInfo> deviceQueueCreateInfo;
		std::vector<float> priority = { 0.5, 0.5, 0.5 }; //优先级
		uint32_t index = 0;
	};

	//Vulkan队列家族类
	class VulkanQueueFamily
	{
	public:
		VulkanQueueFamilyInfo info;

	public:
		VulkanQueueFamily(vk::raii::PhysicalDevice& physicalDevice);
		~VulkanQueueFamily();

		//绑定到逻辑设备
		void BindToLogicalDevice(vk::raii::Device& logicalDevice, vk::raii::Queue& queueType, uint32_t queueIndex);

		//打印基本信息
		void printBaseInfo();

	private:
		//创建队列家族
		void CreateQueueFamily(
			uint32_t& queueIndex
		);

		//查找是否支持队列家族
		void CheckQueueFamilySupported(
			vk::raii::PhysicalDevice& physicalDevice,
			vk::QueueFlagBits flag,
			uint32_t& queueIndex
		);
	};
}
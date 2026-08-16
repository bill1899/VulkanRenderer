//Vulkan实例创建类

#pragma once
#include<iostream>
#include<vector>
#include<ranges>
#include<string>
#include<GLFW/glfw3.h>
#include<vulkan/vulkan_raii.hpp>

#ifdef NDEBUG
constexpr bool enableValidatorLayers = false;
#else
constexpr bool enableValidatorLayers = true;
#endif // NDEBUG

//命名空间避免污染变量名
namespace me {
	//Vulkan实例类创建信息的属性结构体
	struct VulkanInstanceInfo {
		vk::raii::Context  context;                      //Vulkan上下文
		vk::raii::Instance instance = nullptr;           //Vulkan实例对象
		vk::ApplicationInfo app;
		std::vector<const char*> requiredLayers;         //Vulkan需求层容器
		std::vector<const char*> requiredExtensions;     //Vulkan扩展容器
		std::vector<const char*> requiredValidation = { "VK_LAYER_KHRONOS_validation" };
	};

	//Vulkan实例类
	class VulkanInstance
	{
	public:
		VulkanInstanceInfo info;

	public:
		VulkanInstance(
			std::string applicationName,
			uint32_t    applicationVersion,
			std::string engineName,
			uint32_t    engineVersion,
			uint32_t    apiVersion,
			const void* pNext = nullptr
		);
		~VulkanInstance();

	private:
		//创建Vulkan实例
		void CreateVulkanInstance();

		//创建Vulkan实例属性
		void CreateInstanceProperty(
			std::string applicationName,
			uint32_t    applicationVersion,
			std::string engineName,
			uint32_t    engineVersion,
			uint32_t    apiVersion,
			const void* pNext = nullptr
		);

		//检查Vulkan需求层支持
		void CheckRequiredLayers();

		//检查Vulkan扩展层支持
		void CheckRequiredExtensions();

		//辅助函数：用于获取Vulkan具体的扩展名称
		std::vector<const char*> getRequiredExtensions();
	};
}

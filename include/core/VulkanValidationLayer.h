//Vulkan验证层

#include<iostream>
#include<vulkan/vulkan_raii.hpp>

//前向声明，减少编译时间
class VulkanInstance;

namespace me {

	//Vulkan验证层信息属性结构体
	struct VulkanValidationLayerInfo {
		vk::raii::DebugUtilsMessengerEXT validationLayer = nullptr; //Vulkan验证层
	};

	//Vulkan验证层类
	class VulkanValidationLayer
	{
	public:
		VulkanValidationLayerInfo info;

	public:
		VulkanValidationLayer(vk::raii::Instance& instance);
		~VulkanValidationLayer();

	private:
		//创建Vulkan验证层信息
		void setupDebugMessage(vk::raii::Instance& instance);

		//设置验证层错误输出等级
		vk::DebugUtilsMessageSeverityFlagsEXT setupSeverityFlags();

		//设置验证层输出信息类型
		vk::DebugUtilsMessageTypeFlagsEXT setupTypeFlags();

		//验证层信息输出回调函数
		static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
			vk::DebugUtilsMessageSeverityFlagsEXT  severity,
			vk::DebugUtilsMessageTypeFlagsEXT      type,
			const vk::DebugUtilsMessengerCallbackDataEXT* callbackData,
			void* 
		);
	};
}

#include<core/VulkanValidationLayer.h>
#include<core/VulkanInstance.h>

me::VulkanValidationLayer::VulkanValidationLayer(vk::raii::Instance& instance)
{
	setupDebugMessage(instance);
}

me::VulkanValidationLayer::~VulkanValidationLayer()
{

}

void me::VulkanValidationLayer::setupDebugMessage(vk::raii::Instance& instance)
{
	//判断debug模式是否启动
	if (!enableValidatorLayers) {
		return;
	}

	//填充验证层调试信息结构体
	vk::DebugUtilsMessengerCreateInfoEXT debugMessengerInfo = {
		debugMessengerInfo.flags = {},
		debugMessengerInfo.messageSeverity = setupSeverityFlags(),
		debugMessengerInfo.messageType = setupTypeFlags(),
		debugMessengerInfo.pfnUserCallback = vk::PFN_DebugUtilsMessengerCallbackEXT(debugCallback),
		debugMessengerInfo.pUserData = nullptr,
		debugMessengerInfo.pNext = nullptr,
	};

	//判断验证层调试信息是否创建成功
	info.validationLayer = vk::raii::DebugUtilsMessengerEXT(instance, debugMessengerInfo);
	if (info.validationLayer == nullptr) {
		std::cerr << "Vulkan debugMessage Create failed!" << std::endl;
		return;
	}
	else
	{
		std::clog << "Vulkan debugMessage Create success!" << std::endl;
	}
}

vk::DebugUtilsMessageSeverityFlagsEXT me::VulkanValidationLayer::setupSeverityFlags()
{
	//配置验证层输出信息的危险等级
	vk::DebugUtilsMessageSeverityFlagsEXT severity(
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
	);

	return severity;
}

vk::DebugUtilsMessageTypeFlagsEXT me::VulkanValidationLayer::setupTypeFlags()
{
	//配置验证层输出信息的类型
	vk::DebugUtilsMessageTypeFlagsEXT type(
		vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral    |
		vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
		vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance 
	);

	return type;
}

VKAPI_ATTR vk::Bool32 VKAPI_CALL me::VulkanValidationLayer::debugCallback(
	vk::DebugUtilsMessageSeverityFlagsEXT         severity,
	vk::DebugUtilsMessageTypeFlagsEXT             type,
	const vk::DebugUtilsMessengerCallbackDataEXT* callbackData,
	void*
)
{
	if (severity.operator&(vk::DebugUtilsMessageSeverityFlagBitsEXT::eError) != vk::DebugUtilsMessageSeverityFlagsEXT{} ||
		severity.operator&(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) != vk::DebugUtilsMessageSeverityFlagsEXT{} ||
		severity.operator&(vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo) != vk::DebugUtilsMessageSeverityFlagsEXT{}) {
		std::cerr << "validation layer: type" << to_string(type) << " severity: "<< to_string(severity) << " msg: " << callbackData->pMessage << std::endl;
	}

	return vk::False;
}

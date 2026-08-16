
#include <core/VulkanPhysicalDevice.h>

me::VulkanPhysicalDevice::VulkanPhysicalDevice(vk::raii::Instance& instance)
{
	pickPhysicalDevice(instance);
}

me::VulkanPhysicalDevice::~VulkanPhysicalDevice()
{

}

void me::VulkanPhysicalDevice::pickPhysicalDevice(vk::raii::Instance& instance)
{
	//查找电脑设备的显卡类型、数量
	auto devices = instance.enumeratePhysicalDevices();
	if (devices.empty()) {
		throw std::runtime_error("Failed to find GPUs with Vulkan support!");
	}

	//创建一个迭代器，使用C++20的范围库逐个查找我们需要的物理属性是否受支持
	const auto Iter = std::ranges::find_if(devices,
		[&](auto& device) {
			bool supportedVulkan1_4   = CheckVersionSupported(device);       //查找设备是否支持Vulkan1.4版本
			bool supportedQueueFamily = CheckQueueSupported(device);         //查找设备支持的队列家族数量
			bool supportedExtensions  = CheckExtensionSupported(device);     //查找需要使用的到的扩展层支持
			bool supportedFeatures    = CheckDeviceFeaturesSupported(device);//检查设备的特性支持

			return supportedVulkan1_4 && supportedQueueFamily && supportedExtensions && supportedFeatures;
		}
	);

	if (Iter != devices.end()) {
		info.physicalDevice = *Iter;
		std::clog << "Vulkan physical device pick success!" << std::endl;
	}
	else {
		throw std::runtime_error("Failed to find a suitable GPU!");
	}
}

bool me::VulkanPhysicalDevice::CheckVersionSupported(vk::raii::PhysicalDevice& device)
{
	if (device.getProperties().apiVersion >= VK_API_VERSION_1_4) {
		return true;
	}
	else {
		return false;
	}
}

bool me::VulkanPhysicalDevice::CheckQueueSupported(vk::raii::PhysicalDevice& device)
{
	auto queueFamilies = device.getQueueFamilyProperties();
	bool result = std::ranges::any_of(queueFamilies,
		[](const auto& qfp) {
			vk::QueueFlags queueRequired = vk::QueueFlagBits::eGraphics | vk::QueueFlagBits::eTransfer | vk::QueueFlagBits::eCompute;
			bool hasAllType = (qfp.queueFlags & queueRequired) == queueRequired;
			return !!hasAllType;
		}
	);
	return result;
}

bool me::VulkanPhysicalDevice::CheckExtensionSupported(vk::raii::PhysicalDevice& device)
{
	auto deviceExtensions = device.enumerateDeviceExtensionProperties();
	bool result = std::ranges::all_of(info.requiredExtensions, 
		[&deviceExtensions](const auto& requiredDeviceExtension) {
			return std::ranges::any_of(deviceExtensions,
				[requiredDeviceExtension](const auto& deviceExtension) {
					return strcmp(deviceExtension.extensionName, requiredDeviceExtension) == 0;
				}
			);
		}
	);
	
	return result;
}

bool me::VulkanPhysicalDevice::CheckDeviceFeaturesSupported(vk::raii::PhysicalDevice& device)
{
	vk::StructureChain<
		vk::PhysicalDeviceFeatures2,
		vk::PhysicalDeviceVulkan13Features,
		vk::PhysicalDeviceVulkan14Features,
		vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT
	> features = device.getFeatures2<
		vk::PhysicalDeviceFeatures2,
		vk::PhysicalDeviceVulkan13Features,
		vk::PhysicalDeviceVulkan14Features,
		vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();

	auto& features2                    = features.get<vk::PhysicalDeviceFeatures2>();
	auto& vulkan13Features             = features.get<vk::PhysicalDeviceVulkan13Features>();
	auto& dynamicExtendedStateFeatures = features.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();

	bool result = features2.features.samplerAnisotropy &&
		          vulkan13Features.dynamicRendering    &&
		          dynamicExtendedStateFeatures.extendedDynamicState;

	return result;
}


#include <core/VulkanLogicalDevice.h>
#include<core/VulkanQueueFamily.h>

me::VulkanLogicalDevice::VulkanLogicalDevice(vk::raii::PhysicalDevice& physicalDevice, std::vector<const char*> requiredExtensions)
{
	enableFeatures();
	CreateQueueFamily(physicalDevice);
	CreateLogicalDevice(physicalDevice, requiredExtensions);
	info.queue->BindToLogicalDevice(this->info.logicalDevice, info.queue->info.graphicsQueue, info.queue->info.graphicsQueueIndex);
	info.queue->BindToLogicalDevice(this->info.logicalDevice, info.queue->info.transferQueue, info.queue->info.transferQueueIndex);
	info.queue->BindToLogicalDevice(this->info.logicalDevice, info.queue->info.computeQueue, info.queue->info.computeQueueIndex);
}

me::VulkanLogicalDevice::~VulkanLogicalDevice()
{
		
}

void me::VulkanLogicalDevice::CreateLogicalDevice(vk::raii::PhysicalDevice& physicalDevice, std::vector<const char*> requiredExtensions)
{
	setupLogicalDevice(physicalDevice, requiredExtensions);
}

void me::VulkanLogicalDevice::CreateQueueFamily(vk::raii::PhysicalDevice& physicalDevice)
{
	info.queue = std::make_unique<VulkanQueueFamily>(physicalDevice);
}

void me::VulkanLogicalDevice::setupLogicalDevice(vk::raii::PhysicalDevice& physicalDevice, std::vector<const char*> requiredExtensions)
{
	vk::DeviceCreateInfo device = {
		device.flags = {},
		device.queueCreateInfoCount = static_cast<uint32_t>(info.queue->info.deviceQueueCreateInfo.size()),
		device.pQueueCreateInfos = info.queue->info.deviceQueueCreateInfo.data(),
		device.enabledLayerCount = 0,
		device.ppEnabledLayerNames = nullptr,
		device.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size()),
		device.ppEnabledExtensionNames = requiredExtensions.data(),
		device.pEnabledFeatures = nullptr,
		device.pNext = info.enableFeatures.get(),
	};

	std::cout << info.queue->info.deviceQueueCreateInfo.size() << std::endl;

	/*info.deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(requiredExtensions.size());
	info.deviceCreateInfo.ppEnabledExtensionNames = requiredExtensions.data();*/

	info.logicalDevice = vk::raii::Device(physicalDevice, device);
	if (info.logicalDevice == nullptr) {
		std::cerr << "Vulkan logical device and queue Create failed!";
		return;
	}
	else {
		std::clog << "Vulkan logical device and queue Create success!" << std::endl;
	}
}

void me::VulkanLogicalDevice::enableFeatures()
{
	vk::StructureChain <
		vk::PhysicalDeviceFeatures2,
		vk::PhysicalDeviceVulkan13Features,
		vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT
	> features;

	auto& baseFeatures = features.get<vk::PhysicalDeviceFeatures2>();
	auto& vulkan13Features = features.get <vk::PhysicalDeviceVulkan13Features>();
	auto& dynamicFeatures = features.get<vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT>();

	baseFeatures.features.samplerAnisotropy = VK_TRUE;
	vulkan13Features.dynamicRendering = VK_TRUE;
	vulkan13Features.synchronization2 = VK_TRUE;
	dynamicFeatures.extendedDynamicState = VK_TRUE;

	info.enableFeatures = features;
};

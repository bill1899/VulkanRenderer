
#include<core/VulkanQueueFamily.h>

me::VulkanQueueFamily::VulkanQueueFamily(vk::raii::PhysicalDevice& physicalDevice)
{
	CheckQueueFamilySupported(physicalDevice, vk::QueueFlagBits::eGraphics, info.graphicsQueueIndex);
	CheckQueueFamilySupported(physicalDevice, vk::QueueFlagBits::eTransfer, info.transferQueueIndex);
	CheckQueueFamilySupported(physicalDevice, vk::QueueFlagBits::eCompute, info.computeQueueIndex);

	CreateQueueFamily(info.graphicsQueueIndex);

	printBaseInfo();
}

me::VulkanQueueFamily::~VulkanQueueFamily()
{
	info.deviceQueueCreateInfo.clear();
	info.priority.clear();
	info.index = 0;
}

void me::VulkanQueueFamily::BindToLogicalDevice(vk::raii::Device& logicalDevice, vk::raii::Queue& queueType, uint32_t queueIndex)
{
	
	queueType = vk::raii::Queue(logicalDevice, queueIndex, info.index);
	if (queueType == nullptr) {
		std::cerr << "Vulkan queue Create failed!";
		return;
	}
	else {
		std::clog << "Vulkan queue Create success!" << std::endl;
	}
	info.index++;
}

void me::VulkanQueueFamily::printBaseInfo()
{
	std::cout << "graphicsQueueIndex = " << info.graphicsQueueIndex << std::endl;
	std::cout << "transferQueueIndex = " << info.transferQueueIndex << std::endl;
	std::cout << "computeQueueIndex = " << info.computeQueueIndex << std::endl;
	std::cout << "deviceQueueSize = " << info.deviceQueueCreateInfo.size() << std::endl;
}

void me::VulkanQueueFamily::CreateQueueFamily(
	uint32_t& queueIndex
)
{
	/**
	* @brief 队列创建信息结构体（DeviceQueueCreateInfo）
	* 【核心坑点】
	* 1. 同一queueFamilyIndex只能对应1个该结构体，多队列通过queueCount设置，不可重复创建
	* 2. sType必须显式设为eDeviceQueueCreateInfo，不可缺省
	* 3. pQueuePriorities指向的数组生命周期需覆盖vkCreateDevice，且值需在[0.0,1.0]
	* 4. queueFamilyIndex需校验：<队列族总数 + 支持目标功能（图形/传输/计算） 
	*/
	vk::DeviceQueueCreateInfo queue = {
		queue.flags = {},
		queue.queueFamilyIndex = queueIndex,
		queue.queueCount = info.queueCount,
		queue.pQueuePriorities = info.priority.data(),
		queue.pNext = nullptr,
	};
	info.deviceQueueCreateInfo.push_back(std::move(queue));
}

void me::VulkanQueueFamily::CheckQueueFamilySupported(
	vk::raii::PhysicalDevice& physicalDevice,
	vk::QueueFlagBits flag,
	uint32_t& queueIndex
)
{
	std::vector<vk::QueueFamilyProperties> queue = physicalDevice.getQueueFamilyProperties();

	auto queueProperty = std::ranges::find_if(queue,
		[flag](const auto& qfp) {
			return (qfp.queueFlags & flag) == flag;
		}
	);

	queueIndex = static_cast<uint32_t>(std::distance(queue.begin(), queueProperty));
}

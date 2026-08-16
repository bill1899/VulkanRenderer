
#include<core/VulkanCommandBuffer.h>
#include<core/VulkanQueueFamily.h>

me::VulkanCommandBuffer::VulkanCommandBuffer(vk::raii::Device& logicalDevice, uint32_t queueCount, uint32_t queueFamilyIndex)
{
	info.commandBufferCount = queueCount;
	CreateCommandPool(logicalDevice, queueFamilyIndex);
	CreateCommandBuffer(logicalDevice);
}

me::VulkanCommandBuffer::~VulkanCommandBuffer()
{
	info.commandBufferMap.clear();
	info.type.clear();
	info.pool.clear();
}

void me::VulkanCommandBuffer::CreateCommandBuffer(vk::raii::Device& logicalDevice)
{
	for (int i = 0; i < info.commandBufferCount; ++i) {
		vk::CommandBufferAllocateInfo commandBuffer = {
		commandBuffer.commandPool = *info.pool[info.type[i]],
		commandBuffer.level = vk::CommandBufferLevel::ePrimary,
		commandBuffer.commandBufferCount = MAX_FRAME_FIGHT,
		commandBuffer.pNext = nullptr,
		};
		//info.commandBuffers.push_back(std::move(vk::raii::CommandBuffers(logicalDevice, commandBuffer)));
		info.commandBufferMap[info.type[i]] = std::make_unique<vk::raii::CommandBuffers>(logicalDevice, commandBuffer);
	}

	if (info.commandBufferMap.empty()) {
		std::cerr << "Vulkan command pool Create failed!" << std::endl;
		return;
	}
	else {
		std::clog << "Vulkan command pool Create success!  " << "current commandBuffer Count:" << info.commandBufferMap.size() << std::endl;
	}
}

void me::VulkanCommandBuffer::CreateCommandPool(vk::raii::Device& logicalDevice, uint32_t queueFamilyIndex)
{
	for (int i = 0; i < info.commandBufferCount; ++i) {
		vk::CommandPoolCreateInfo commandPool = {
		commandPool.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
		commandPool.queueFamilyIndex = queueFamilyIndex,
		commandPool.pNext = nullptr,
		};

		info.pool[info.type[i]] = std::make_unique<vk::raii::CommandPool>(std::move(vk::raii::CommandPool(logicalDevice, commandPool)));
	}
}

vk::CommandBuffer me::VulkanCommandBuffer::beginRecord(
	vk::raii::Device& logicalDevice,
	VulkanCommandBufferType type, 
	vk::CommandBufferUsageFlagBits flag,
	vk::CommandBufferInheritanceInfo Inheritance
)
{
	vk::CommandBufferBeginInfo begin = {
		begin.flags = flag,
		begin.pInheritanceInfo = &Inheritance,
		begin.pNext = nullptr,
	};

	auto& commandBuffer = *info.commandBufferMap.find(type)->second.get()->at(0);
	commandBuffer.begin(begin);

	return commandBuffer;
}

void me::VulkanCommandBuffer::endRecord(vk::CommandBuffer& commandBuffer)
{
	commandBuffer.end();
}
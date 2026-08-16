
#include<core/VulkanBuffer.h>
#include<core/VulkanDescription.h>
#include<core/VulkanCommandBuffer.h>
#include<core/VulkanQueueFamily.h>

me::VulkanBuffer::VulkanBuffer(
	vk::raii::PhysicalDevice* physicalDevice,
	vk::raii::Device* logicalDevice,
	VulkanCommandBuffer* commandBuffer,
	VulkanQueueFamily* queue
)
{
	info.physicalDevice = physicalDevice;
	info.logicalDevice = logicalDevice;
	info.commandBuffer = commandBuffer;
	info.queue = queue;
}

me::VulkanBuffer::~VulkanBuffer()
{
	info.commandBuffer = nullptr;
	info.logicalDevice = nullptr;
	info.physicalDevice = nullptr;
	info.queue = nullptr;
}


vk::raii::Buffer& me::VulkanBuffer::CreateVertexBuffer(const std::vector<Vertex>& vertices)
{
	vk::DeviceSize size = sizeof(vertices[0]) * vertices.size();
	vk::raii::Buffer stagingBuffer({});
	vk::raii::DeviceMemory stagingMemory({});
	CreateBuffer(
		size, 
		vk::BufferUsageFlagBits::eTransferSrc, 
		vk::SharingMode::eExclusive, 
		vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible,
		stagingBuffer,
		stagingMemory
	);

	void* data = stagingMemory.mapMemory(0, size);
	memcpy(data, vertices.data(), size);
	stagingMemory.unmapMemory();
	
	CreateBuffer(
		size,
		vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst,
		vk::SharingMode::eExclusive,
		vk::MemoryPropertyFlagBits::eDeviceLocal,
		info.buffer, 
		info.memory
	);

	copyBuffer(stagingBuffer, info.buffer, size);
	std::cout << "vertexBuffer create success!" << std::endl;
	return info.buffer;
}


vk::raii::Buffer& me::VulkanBuffer::CreateIndexBuffer(const std::vector<uint16_t>& indices)
{
	vk::DeviceSize size = sizeof(indices[0]) * indices.size();
	vk::raii::Buffer stagingBuffer({});
	vk::raii::DeviceMemory stagingMemory({});
	CreateBuffer(
		size,
		vk::BufferUsageFlagBits::eTransferSrc,
		vk::SharingMode::eExclusive,
		vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible,
		stagingBuffer,
		stagingMemory
	);

	void* data = stagingMemory.mapMemory(0, size);
	memcpy(data, indices.data(), size);
	stagingMemory.unmapMemory();

	CreateBuffer(
		size,
		vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst,
		vk::SharingMode::eExclusive,
		vk::MemoryPropertyFlagBits::eDeviceLocal,
		info.buffer,
		info.memory
	);

	copyBuffer(stagingBuffer, info.buffer, size);
	std::cout << "indexBuffer create success!" << std::endl;
	return info.buffer;
}



vk::raii::Buffer& me::VulkanBuffer::CreateUniformBuffer(uint32_t dataSize)
{
	vk::DeviceSize size = dataSize;
	vk::raii::Buffer stagingBuffer({});
	vk::raii::DeviceMemory stagingMemory({});
	CreateBuffer(
		size,
		vk::BufferUsageFlagBits::eTransferSrc,
		vk::SharingMode::eExclusive,
		vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible,
		stagingBuffer,
		stagingMemory
	);
	return stagingBuffer;
	// TODO: 在此处插入 return 语句
}


void me::VulkanBuffer::CreateBuffer(
	vk::DeviceSize size,
	vk::BufferUsageFlags usage,
	vk::SharingMode mode,
	vk::MemoryPropertyFlags flags,
	vk::raii::Buffer& buffer,
	vk::raii::DeviceMemory& bufferMemory
)
{
	vk::BufferCreateInfo stagingBuffer = {
		stagingBuffer.flags = {},
		stagingBuffer.size = size,
		stagingBuffer.usage = usage,
		stagingBuffer.sharingMode = mode,
		stagingBuffer.queueFamilyIndexCount = 0,
		stagingBuffer.pQueueFamilyIndices = nullptr,
		stagingBuffer.pNext = nullptr
	};
	buffer =  vk::raii::Buffer(*info.logicalDevice, stagingBuffer);

	vk::MemoryRequirements required = buffer.getMemoryRequirements();
	vk::MemoryAllocateInfo allocate = {
		allocate.allocationSize = required.size,
		allocate.memoryTypeIndex = findMemoryType(required.memoryTypeBits, flags),
		allocate.pNext = nullptr
	};
	bufferMemory = vk::raii::DeviceMemory(*info.logicalDevice, allocate);
	buffer.bindMemory(bufferMemory, 0);
}

uint32_t me::VulkanBuffer::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags flag)
{
	vk::PhysicalDeviceMemoryProperties properties = info.physicalDevice->getMemoryProperties();
	for (uint32_t i = 0; i < properties.memoryTypeCount; i++) {
		if (typeFilter & (1 << i) && (properties.memoryTypes[i].propertyFlags & flag) == flag) {
			return i;
		}
	}
	throw std::runtime_error("failed to find suitable memory type!");
}

void me::VulkanBuffer::copyBuffer(vk::raii::Buffer& srcBuffer, vk::raii::Buffer& dstBuffer, vk::DeviceSize size)
{
	vk::CommandBufferBeginInfo begin = {
		begin.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit,
		begin.pInheritanceInfo = nullptr,
		begin.pNext = nullptr,
	};
	
	auto& commandBuffer = info.commandBuffer->info.commandBufferMap.find(me::VulkanCommandBufferType::TRANSFER)->second.get()->at(0);
	commandBuffer.reset();
	commandBuffer.begin(begin);
	commandBuffer.copyBuffer(srcBuffer, dstBuffer, vk::BufferCopy(0, 0, size));
	commandBuffer.end();

	vk::SubmitInfo submit = {
		submit.waitSemaphoreCount = 0,
		submit.pWaitSemaphores = nullptr,
		submit.pWaitDstStageMask = nullptr,
		submit.commandBufferCount = 1,
		submit.pCommandBuffers = &*commandBuffer,
		submit.signalSemaphoreCount = 0,
		submit.pSignalSemaphores = nullptr,
		submit.pNext = nullptr
	};

	info.queue->info.transferQueue.submit(submit, nullptr);
	info.queue->info.transferQueue.waitIdle();
}
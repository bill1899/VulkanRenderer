//Vulkan缓冲区类

#include<iostream>
#include<vector>
#include<vulkan/vulkan_raii.hpp>

namespace me {

	//前向声明，避免循环依赖，减少编译时间
	struct Vertex;
	class VulkanCommandBuffer;
	class VulkanQueueFamily;

	struct VulkanBufferInfo {
		vk::raii::Buffer buffer = nullptr; 
		vk::raii::DeviceMemory memory = nullptr;
		vk::raii::PhysicalDevice* physicalDevice = nullptr;
		vk::raii::Device*         logicalDevice  = nullptr;
		me::VulkanCommandBuffer* commandBuffer = nullptr;
		me::VulkanQueueFamily*   queue          = nullptr;
	};

	class VulkanBuffer
	{
	public:
		VulkanBufferInfo info;

	public:
		VulkanBuffer(
			vk::raii::PhysicalDevice* physicalDevice,
			vk::raii::Device* logicalDevice,
			VulkanCommandBuffer* commandBuffer,
			VulkanQueueFamily* queue
		);
		~VulkanBuffer();

		//创建顶点缓冲区
		vk::raii::Buffer& CreateVertexBuffer(const std::vector<Vertex>& vertices);

		//创建索引缓冲区
		vk::raii::Buffer& CreateIndexBuffer(const std::vector<uint16_t>& indices);

		//创建uniform缓冲区
		vk::raii::Buffer& CreateUniformBuffer(uint32_t size);
		
	private:
		//创建缓冲区
		void CreateBuffer(
			vk::DeviceSize size,
			vk::BufferUsageFlags usage,
			vk::SharingMode mode,
			vk::MemoryPropertyFlags flags,
			vk::raii::Buffer& buffer,
			vk::raii::DeviceMemory& bufferMemory
		);

		//查找内存属性
		uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags flag);
	
		//复制缓冲区
		void copyBuffer(vk::raii::Buffer& srcBuffer, vk::raii::Buffer& dstBuffer, vk::DeviceSize size);
	};
}
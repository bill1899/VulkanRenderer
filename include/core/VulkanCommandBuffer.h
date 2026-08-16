//Vulkan命令缓冲区

#include<iostream>
#include<unordered_map>
#include<vector>
#include<memory>
#include<vulkan/vulkan_raii.hpp>

namespace me {

	class VulkanQueueFamily;

	constexpr int MAX_FRAME_FIGHT = 2;

	enum class VulkanCommandBufferType
	{
		GRAPHICS = 0, //图形队列命令缓冲区
		TRANSFER = 1, //传输队列命令缓冲区
		COMPUTE  = 2, //计算队列命令缓冲区
	};

	struct VulkanCommandBufferInfo {
		uint32_t commandBufferCount = 0;  //命令缓冲区的数量
		std::unordered_map<VulkanCommandBufferType, std::unique_ptr<vk::raii::CommandBuffers>> commandBufferMap;
		std::vector<VulkanCommandBufferType> type = {
			VulkanCommandBufferType::GRAPHICS,
			VulkanCommandBufferType::TRANSFER,
			VulkanCommandBufferType::COMPUTE,
		};
		std::unordered_map<VulkanCommandBufferType, std::unique_ptr<vk::raii::CommandPool>> pool;
	};

	class VulkanCommandBuffer
	{
	public:
		VulkanCommandBufferInfo info;

	public:
		VulkanCommandBuffer(vk::raii::Device& logicalDevice, uint32_t queueCount, uint32_t queueFamilyIndex);
		~VulkanCommandBuffer();

		//录制命令
		vk::CommandBuffer beginRecord(
			vk::raii::Device& logicalDevice,
			VulkanCommandBufferType type,
			vk::CommandBufferUsageFlagBits flag,
			vk::CommandBufferInheritanceInfo Inheritance = {}
		);

		//结束录制
		void endRecord(vk::CommandBuffer& commandBuffer);

		//

	private:
		//创建命令缓冲区
		void CreateCommandBuffer(vk::raii::Device& logicalDevice);

		//创建命令池
		void CreateCommandPool(vk::raii::Device& logicalDevice, uint32_t queueFamilyIndex);
	};
}
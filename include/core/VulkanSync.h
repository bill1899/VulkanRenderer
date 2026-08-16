//Vulkan同步原语

#include<iostream>
#include<vector>
#include<vulkan/vulkan_raii.hpp>


namespace me {

	class VulkanSwapChain;

	struct VulkanSyncInfo {
		vk::raii::Device* logicalDevice = nullptr;
		VulkanSwapChain* swapChain = nullptr;
		std::vector<vk::raii::Semaphore> renderFinishedSemaphores = {};
		std::vector<vk::raii::Semaphore> presentCompleteSemaphores = {};
		std::vector<vk::raii::Fence> fences = {};
	};

	class VulkanSync
	{
	public:
		VulkanSyncInfo info;

	public:
		VulkanSync(vk::raii::Device* logicalDevice, me::VulkanSwapChain* swapChain);
		~VulkanSync();

		//创建图像屏障变换
		void transition_Image_layout(
			vk::Image image,
			vk::PipelineStageFlags2 src_stage_mask,
			vk::PipelineStageFlags2 dst_stage_mask,
			vk::AccessFlagBits2 src_access_mask,
			vk::AccessFlagBits2 dst_access_mask,
			vk::ImageLayout old_layout,
			vk::ImageLayout new_layout,
			vk::ImageAspectFlags image_aspect,
			vk::raii::CommandBuffers& commandBuffer
		);

		void CreateSync();

	private:
		//创建栅栏
		void CreateSemaphore();

		//创建栅栏
		void CreateFence();
	};
}
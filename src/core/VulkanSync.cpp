
#include<core/VulkanSync.h>
#include<core/VulkanSwapChain.h>
#include<core/VulkanCommandBuffer.h>

me::VulkanSync::VulkanSync(vk::raii::Device* logicalDevice, me::VulkanSwapChain* swapChain)
{
	info.logicalDevice = logicalDevice;
	info.swapChain = swapChain;
}

me::VulkanSync::~VulkanSync()
{
	info.fences.clear();
	info.presentCompleteSemaphores.clear();
	info.renderFinishedSemaphores.clear();
}


void me::VulkanSync::CreateSync()
{
	CreateSemaphore();
	CreateFence();
}


void me::VulkanSync::transition_Image_layout(
	vk::Image image,
	vk::PipelineStageFlags2 src_stage_mask,
	vk::PipelineStageFlags2 dst_stage_mask,
	vk::AccessFlagBits2 src_access_mask,
	vk::AccessFlagBits2 dst_access_mask,
	vk::ImageLayout old_layout,
	vk::ImageLayout new_layout,
	vk::ImageAspectFlags image_aspect,
	vk::raii::CommandBuffers& commandBuffers
)
{
	vk::ImageSubresourceRange subResource = {
		subResource.aspectMask = image_aspect,
		subResource.baseMipLevel = 0,
		subResource.levelCount = 1,
		subResource.baseArrayLayer = 0,
		subResource.layerCount = 1
	};
	vk::ImageMemoryBarrier2 barrier = {
		barrier.srcStageMask = src_stage_mask,
		barrier.srcAccessMask = src_access_mask,
		barrier.dstStageMask = dst_stage_mask,
		barrier.dstAccessMask = dst_access_mask,
		barrier.oldLayout = old_layout,
		barrier.newLayout = new_layout,
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		barrier.image = image,
		barrier.subresourceRange = subResource,
		barrier.pNext = nullptr
	};

	vk::DependencyInfo depend = {
		depend.dependencyFlags = vk::DependencyFlagBits::eByRegion,
		depend.memoryBarrierCount = 0,
		depend.pMemoryBarriers = nullptr,
		depend.bufferMemoryBarrierCount = 0,
		depend.pBufferMemoryBarriers = nullptr,
		depend.imageMemoryBarrierCount = 1,
		depend.pImageMemoryBarriers = &barrier,
		depend.pNext = nullptr
	};

	commandBuffers[info.swapChain->info.frameIndex].pipelineBarrier2(depend);
}


void me::VulkanSync::CreateSemaphore()
{
	for (int i = 0; i < info.swapChain->info.swapChainImages.size(); ++i) {
		info.renderFinishedSemaphores.emplace_back(vk::raii::Semaphore(*info.logicalDevice, vk::SemaphoreCreateInfo()));
	}

	for (int i = 0; i < MAX_FRAME_FIGHT; ++i) {
		info.presentCompleteSemaphores.emplace_back(vk::raii::Semaphore(*info.logicalDevice, vk::SemaphoreCreateInfo()));
	}
}

void me::VulkanSync::CreateFence()
{
	for (int i = 0; i < MAX_FRAME_FIGHT; ++i) {
		vk::FenceCreateInfo fence = {
			fence.flags = vk::FenceCreateFlagBits::eSignaled,
			fence.pNext = nullptr
		};
		info.fences.emplace_back(*info.logicalDevice, fence);
	}
}
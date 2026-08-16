
/*
CMake编译指令当前目录编译：cmake -G "Visual Studio 17 2022" -A x64/x32
CMake编译指令指定目录编译：cmake -S. -B 指定目录路径 -G "Visual Studio 17 2022" -A x64/x32

Vulkan Spir-v着色器编译指令：glslangValidator -V 着色器源文件 -o spir-v着色器源文件 --target-env Vulkan版本
*/

#include<iostream>
#include<string_view>
#include<stdexcept>
#include<map>
#include<filesystem>
#include<core/VulkanInstance.h>
#include<core/GLFWwindow.h>
#include<core/VulkanValidationLayer.h>
#include<core/VulkanPhysicalDevice.h>
#include<core/VulkanLogicalDevice.h>
#include<core/VulkanQueueFamily.h>
#include<core/VulkanSurface.h>
#include<core/VulkanSwapChain.h>
#include<core/VulkanDescription.h>
#include<core/VulkanGraphicsPipeline.h>
#include<core/VulkanCommandBuffer.h>
#include<core/VulkanBuffer.h>
#include<core/VulkanSync.h>

void init();
void recordCommandBuffer(me::VulkanCommandBuffer& commandBuffer, me::VulkanSync& sync, me::VulkanSwapChain& swapChain, me::VulkanGraphicsPipeline& graphicsPipeline, vk::raii::Buffer& vertexBuffer, vk::raii::Buffer& indexBuffer,uint32_t imageIndex);
void DrawFrame(me::VulkanLogicalDevice& logicalDevice, me::VulkanSwapChain& swapChain, me::VulkanCommandBuffer& commandBuffer, me::VulkanSync& sync, me::VulkanGraphicsPipeline& graphicsPipeline, vk::raii::Buffer& vertexBuffer, vk::raii::Buffer& indexBuffer);
const std::string_view vertex = "E:/LearnFile/C++/C++扩展/Vulkan/VulkanEngine/asset/spirv/vertexShader.spv";
const std::string_view fragment = "E:/LearnFile/C++/C++扩展/Vulkan/VulkanEngine/asset/spirv/fragmentShader.spv";

const std::vector<me::Vertex> vertices = {
	{{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}},
	{{0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}},
	{{0.5f, 0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}},
	{{-0.5f, 0.5f, 0.0f}, {0.0f, 1.0f, 1.0f}},
};

const std::vector<uint16_t> indices = {
	0, 1, 2, 2, 3, 0,
};

struct MVPMatrix
{
	alignas(16) glm::mat4 projection;
	alignas(16) glm::mat4 view;
	alignas(16) glm::mat4 model;
};


int main()
{
	std::cout << std::filesystem::current_path() << std::endl;;
	init();

	std::cin.get();
	return 0;
}

void recordCommandBuffer(
	me::VulkanCommandBuffer& commandBuffer,
	me::VulkanSync& sync,
	me::VulkanSwapChain& swapChain,
	me::VulkanGraphicsPipeline& graphicsPipeline,
	vk::raii::Buffer& vertexBuffer,
	vk::raii::Buffer& indexBuffer,
	uint32_t imageIndex)
{
	auto& graphicsBuffer = *commandBuffer.info.commandBufferMap.find(me::VulkanCommandBufferType::GRAPHICS)->second.get();
	graphicsBuffer[swapChain.info.frameIndex].begin({});

	sync.transition_Image_layout(
		swapChain.info.swapChainImages[imageIndex],
		vk::PipelineStageFlagBits2::eColorAttachmentOutput,
		vk::PipelineStageFlagBits2::eColorAttachmentOutput,
		vk::AccessFlagBits2::eNone,
		vk::AccessFlagBits2::eColorAttachmentWrite,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eColorAttachmentOptimal,
		vk::ImageAspectFlagBits::eColor,
		graphicsBuffer
	);

	vk::ClearValue clearColor = vk::ClearColorValue(0.1f, 0.1f, 0.1f, 1.0f);
	
	vk::RenderingAttachmentInfo attachment = {};
	attachment.imageView = swapChain.info.swapChainImageViews[imageIndex],
	attachment.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
	attachment.resolveMode = vk::ResolveModeFlagBits::eNone;
	attachment.resolveImageView = vk::ImageView{};
	attachment.resolveImageLayout = vk::ImageLayout::eUndefined;
	attachment.loadOp = vk::AttachmentLoadOp::eClear;
	attachment.storeOp = vk::AttachmentStoreOp::eStore;
	attachment.clearValue = clearColor;
	attachment.pNext = nullptr;

	vk::RenderingInfo rendering = {};
	rendering.flags = {};
	rendering.renderArea = vk::Rect2D({ 0, 0 }, swapChain.info.swapChainExtent),
	rendering.layerCount = 1;
	rendering.viewMask = 0;
	rendering.colorAttachmentCount = 1;
	rendering.pColorAttachments = &attachment;
	rendering.pDepthAttachment = nullptr;
	rendering.pStencilAttachment = nullptr;
	rendering.pNext = nullptr;

	/*if (rendering.pColorAttachments->imageView == nullptr) {
		std::cerr << "颜色附件数组指针为空！" << std::endl;
		abort();
	}*/

	graphicsBuffer[swapChain.info.frameIndex].beginRendering(rendering);
	graphicsBuffer[swapChain.info.frameIndex].bindPipeline(vk::PipelineBindPoint::eGraphics, *graphicsPipeline.info.graphicsPipeline);
	graphicsBuffer[swapChain.info.frameIndex].setViewport(0, vk::Viewport(0.0f, 0.0f,
		static_cast<float>(swapChain.info.swapChainExtent.width),
		static_cast<float>(swapChain.info.swapChainExtent.height), 0.0f, 1.0f));
	graphicsBuffer[swapChain.info.frameIndex].setScissor(0, vk::Rect2D(vk::Offset2D(0, 0), swapChain.info.swapChainExtent));
	graphicsBuffer[swapChain.info.frameIndex].bindVertexBuffers(0, *vertexBuffer, {0});
	graphicsBuffer[swapChain.info.frameIndex].bindIndexBuffer(*indexBuffer, 0, vk::IndexType::eUint16);
	graphicsBuffer[swapChain.info.frameIndex].drawIndexed(static_cast<uint32_t>(indices.size()), 1, 0, 0, 0);
	graphicsBuffer[swapChain.info.frameIndex].endRendering();

	sync.transition_Image_layout(
		swapChain.info.swapChainImages[imageIndex],
		vk::PipelineStageFlagBits2::eColorAttachmentOutput,
		vk::PipelineStageFlagBits2::eBottomOfPipe,
		vk::AccessFlagBits2::eColorAttachmentWrite,
		vk::AccessFlagBits2::eNone,
		vk::ImageLayout::eColorAttachmentOptimal,
		vk::ImageLayout::ePresentSrcKHR,
		vk::ImageAspectFlagBits::eColor,
		graphicsBuffer
	);
	graphicsBuffer[swapChain.info.frameIndex].end();
}


void DrawFrame(
	me::VulkanLogicalDevice& logicalDevice,
	me::VulkanSwapChain& swapChain,
	me::VulkanCommandBuffer& commandBuffer,
	me::VulkanSync& sync,
	me::VulkanGraphicsPipeline& graphicsPipeline,
	vk::raii::Buffer& vertexBuffer,
	vk::raii::Buffer& indexBuffer
)
{
	auto FenceResult = logicalDevice.info.logicalDevice.waitForFences(*sync.info.fences[swapChain.info.frameIndex], VK_TRUE, UINT64_MAX);
	if (FenceResult != vk::Result::eSuccess) {
		throw std::runtime_error("failed to wait for fence!");
	}

	auto [result, imageIndex] = swapChain.info.swapChain.acquireNextImage(UINT64_MAX, sync.info.presentCompleteSemaphores[swapChain.info.frameIndex], nullptr);
	
	if (result == vk::Result::eErrorOutOfDateKHR) {
		return;
	}
	if ((result != vk::Result::eSuccess) && (result != vk::Result::eSuboptimalKHR)) {
		assert(result == vk::Result::eTimeout || result == vk::Result::eNotReady);
		throw std::range_error("Failed to swap chain image!");
	}

	logicalDevice.info.logicalDevice.resetFences(*sync.info.fences[swapChain.info.frameIndex]);

	commandBuffer.info.commandBufferMap.find(me::VulkanCommandBufferType::GRAPHICS)->second->at(swapChain.info.frameIndex).reset();
	recordCommandBuffer(commandBuffer, sync, swapChain, graphicsPipeline, vertexBuffer, indexBuffer, imageIndex);
	vk::PipelineStageFlags waitDestinationStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput);

	vk::SubmitInfo submit = {
		submit.waitSemaphoreCount = 1,
		submit.pWaitSemaphores = &*sync.info.presentCompleteSemaphores[swapChain.info.frameIndex],
		submit.pWaitDstStageMask = &waitDestinationStageMask,
		submit.commandBufferCount = 1,
		submit.pCommandBuffers = &*commandBuffer.info.commandBufferMap.find(me::VulkanCommandBufferType::GRAPHICS)->second->at(swapChain.info.frameIndex),
		submit.signalSemaphoreCount = 1,
		submit.pSignalSemaphores = &*sync.info.renderFinishedSemaphores[imageIndex],
		submit.pNext = nullptr
	};

	logicalDevice.info.queue->info.graphicsQueue.submit(submit, *sync.info.fences[swapChain.info.frameIndex]);

	vk::PresentInfoKHR present = {
		present.waitSemaphoreCount = 1,
		present.pWaitSemaphores = &*sync.info.renderFinishedSemaphores[imageIndex],
		present.swapchainCount = 1,
		present.pSwapchains = &*swapChain.info.swapChain,
		present.pImageIndices = &imageIndex,
		present.pResults = nullptr,
		present.pNext = nullptr
	};
	
	result = logicalDevice.info.queue->info.graphicsQueue.presentKHR(present);
	if ((result == vk::Result::eSuccess) || (result == vk::Result::eErrorOutOfDateKHR)) {
		swapChain.info.framebufferResized = false;
	}
	else {
		assert(result == vk::Result::eSuccess);
	}

	swapChain.info.frameIndex = (swapChain.info.frameIndex + 1) % me::MAX_FRAME_FIGHT;
}

void init()
{
	me::Window window;
	me::VulkanInstance Instance(
		"VulkanEngine",
		VK_MAKE_VERSION(1, 4, 0),
		"Cloud Engine",
		VK_MAKE_VERSION(1, 0, 0),
		VK_API_VERSION_1_4);
	me::VulkanValidationLayer validationLayer(Instance.info.instance);
	me::VulkanPhysicalDevice physicalDevice(Instance.info.instance);
	me::VulkanLogicalDevice logicalDevice(physicalDevice.info.physicalDevice, physicalDevice.info.requiredExtensions);
	me::VulkanSurface surface(Instance.info.instance, window.info.window);
	me::VulkanSwapChain swapChain(physicalDevice.info.physicalDevice, logicalDevice.info.logicalDevice, surface.info.surface, window.info.window);
	me::Description description(&logicalDevice.info.logicalDevice);
	me::VulkanGraphicsPipeline graphicsPipeline(&logicalDevice.info.logicalDevice, swapChain.info.swapChainFormat, &description);

	graphicsPipeline.CreateShaderStateInfo(vertex.data(), vk::ShaderStageFlagBits::eVertex);
	graphicsPipeline.CreateShaderStateInfo(fragment.data(), vk::ShaderStageFlagBits::eFragment);
	std::cout << "shaderCount = " << graphicsPipeline.info.shaders.size() << std::endl;
	
	me::VulkanDescription::BindVertexDescription(0, vk::VertexInputRate::eVertex, sizeof(me::Vertex));
	std::cout << "descriptionCount = " << me::VulkanDescription::descriptions.size() << std::endl;
	graphicsPipeline.CreateVertexInputStateInfo();

	graphicsPipeline.CreateInputAssemblyStateInfo(vk::PrimitiveTopology::eTriangleList);

	graphicsPipeline.CreateViewportStateInfo();

	graphicsPipeline.CreateRasterizationStateInfo(vk::PolygonMode::eFill, vk::CullModeFlagBits::eNone, vk::FrontFace::eCounterClockwise);

	graphicsPipeline.CreateMultisampleStateInfo(vk::SampleCountFlagBits::e1);

	graphicsPipeline.CreateDepthStencilStateInfo();

	graphicsPipeline.CreateColorBlendStateInfo();

	graphicsPipeline.CreateDynamicRenderingStateInfo();

	graphicsPipeline.CreatePipelineLayoutStateInfo();

	graphicsPipeline.CreateGraphicsPipeline();

	me::VulkanCommandBuffer commandBuffer(logicalDevice.info.logicalDevice, logicalDevice.info.queue->info.queueCount, 0);
	me::VulkanBuffer vertexBuffer(&physicalDevice.info.physicalDevice, &logicalDevice.info.logicalDevice, &commandBuffer, logicalDevice.info.queue.get());
	vertexBuffer.CreateVertexBuffer(vertices);
	me::VulkanBuffer indexBuffer(&physicalDevice.info.physicalDevice, &logicalDevice.info.logicalDevice, &commandBuffer, logicalDevice.info.queue.get());
	indexBuffer.CreateIndexBuffer(indices);

	me::VulkanSync sync(&logicalDevice.info.logicalDevice, &swapChain);
	sync.CreateSync();

	while (!glfwWindowShouldClose(window.info.window))
	{
		glfwPollEvents();
		DrawFrame(logicalDevice, swapChain, commandBuffer, sync, graphicsPipeline, vertexBuffer.info.buffer, indexBuffer.info.buffer);
	}
	logicalDevice.info.logicalDevice.waitIdle();

	glfwDestroyWindow(window.info.window);
	glfwTerminate();
}


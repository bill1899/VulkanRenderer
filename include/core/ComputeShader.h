
#pragma once
#include<iostream>
#include<vector>
#include<string_view>
#include<fstream>
#include<stdexcept>
#include<vulkan/vulkan_raii.hpp>
#include"tgaimage.h"


//全局常量
constexpr std::string_view appName = "ComputeShader";
constexpr std::string_view engineName = "LineDraw";
constexpr std::string_view compute = "../../asset/compute.spv";
constexpr bool IsDebug = false;
const std::vector<const char*> requiredLayers = { "VK_LAYER_KHRONOS_validation" };
const std::vector<const char*> requiredExtension = { vk::EXTDebugUtilsExtensionName };

struct CLine
{
	int ax, ay;
	int bx, by;
	int color[4];
};


class ComputeShaderPipeline
{
public:
	ComputeShaderPipeline(int width, int height, int NUM_LINES, std::vector<CLine> lines, std::vector<uint8_t>& data);
	~ComputeShaderPipeline() = default;

	/*
	* Init
	*/
	void init();

	/*
	* RecordCommandBuffer
	*/
	void recordCommandBuffer();

	/*
	* Run
	*/
	void run();

	/*
	* CreateStagingBuffer
	*/
	void CreateStagingBuffer(vk::raii::Buffer& buffer, vk::raii::DeviceMemory& memory, vk::BufferUsageFlags bufferUsage, int size);

	/*
	* CopyData
	*/
	void copyData(vk::raii::Buffer& srcBuffer, vk::raii::Buffer& dstBuffer, vk::DeviceSize size);

	/*
	* CopyImageData
	*/
	void copyImageData(vk::raii::Image& srcImage, vk::raii::Buffer& dstBuffer, vk::ImageLayout layout);

	/*
	* GetData
	*/
	std::vector<uint8_t> getData();

private:

	void CreateInstance();

	void CreatePhysicalDevice();

	void CreateLogicalDevice();

	void CreateQueueFamily();

	void CreateBuffer();

	void CreateDescriptorSetLayout();

	void CreateImage();

	void CreateImageView();

	void CreatePipeline();

	void CreateCommandBuffer();

	void CreateFence();


	void setupDebugMessage();
	vk::DebugUtilsMessageSeverityFlagsEXT setupSeverityFlags();
	vk::DebugUtilsMessageTypeFlagsEXT setupTypeFlags();
	//验证层信息输出回调函数
	static VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
		vk::DebugUtilsMessageSeverityFlagsEXT  severity,
		vk::DebugUtilsMessageTypeFlagsEXT      type,
		const vk::DebugUtilsMessengerCallbackDataEXT* callbackData,
		void*
	);

	vk::raii::ShaderModule CreateShaderMode(const std::vector<char>& code) const;
	std::vector<char> ReadFile(const std::string& filename);
	void setupPushConstant();
	void setupDescriptorSetLayout();
	void setupDescriptorPool();
	void setupDescriptorSet();
	void writeDescriptorSet();
	void setupCommandPool();
	void setupCommandAllocate();
	vk::Extent3D setupImageExtent3D();
	vk::ComponentMapping setupImageViewComponent();
	vk::ImageSubresourceRange setupImageViewSubresourceRange();
	uint32_t findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags flags);
	void tansfertionImageLayout(
		vk::raii:: Image& image_,
		vk::AccessFlags srcAccessMask,
		vk::AccessFlags dstAccessMask,
		vk::ImageLayout oldLayout,
		vk::ImageLayout newLayout,
		vk::PipelineStageFlags srcStage,
		vk::PipelineStageFlags dstStage
	);
	

	vk::raii::Context                context;
	vk::raii::Instance               instance              = nullptr;
	vk::raii::PhysicalDevice         physicalDevice        = nullptr;
	vk::raii::Device                 logicalDevice         = nullptr;
	vk::raii::Queue                  ComputeQueue          = nullptr;
	vk::raii::Pipeline               computePipeline       = nullptr;
	vk::raii::DebugUtilsMessengerEXT ValidationLayer       = nullptr;
	vk::raii::PipelineLayout         layout                = nullptr;
	vk::raii::ShaderModule           ComputeShader         = nullptr;
	vk::raii::Buffer                 buffer                = nullptr;
	vk::raii::DeviceMemory           memory                = nullptr;
	vk::raii::DescriptorSetLayout    descriptorSetLayout   = nullptr;
	vk::raii::DescriptorPool         descriptorPool        = nullptr;
	vk::raii::DescriptorSet          descriptorSet         = nullptr;
	vk::raii::Image                  image                 = nullptr;
	vk::raii::DeviceMemory           imageMemory           = nullptr;
	vk::raii::ImageView              imageView             = nullptr;
	vk::raii::CommandPool            commandPool           = nullptr;
	vk::raii::CommandBuffers         commandBuffers        = nullptr;
	vk::raii::Fence                  fence                 = nullptr;

	vk::ApplicationInfo appInfo;
	vk::DeviceQueueCreateInfo queueInfo;
	vk::PipelineShaderStageCreateInfo stageInfo;
	vk::PipelineLayoutCreateInfo layoutInfo;
	vk::ShaderModule module;
	vk::PushConstantRange pushConstants;
	

	const float priority = 1.0f;
	const std::vector<const char*> Extensions = {};
	const std::vector<vk::DescriptorType> descriptorType = {
		vk::DescriptorType::eStorageImage,
		vk::DescriptorType::eStorageBuffer,
	};
	std::vector<vk::DescriptorSetLayoutBinding> bindings = {};
	std::vector<vk::DescriptorPoolSize> poolSizes = {};
	std::vector<CLine> m_lines;
	std::vector<uint8_t> m_data;

	int m_width = 0;
	int m_height = 0;
	int m_NUM_LINES = 0;

};
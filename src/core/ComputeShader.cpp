#include "ComputeShader.h"

ComputeShaderPipeline::ComputeShaderPipeline(int width, int height, int NUM_LINES, std::vector<CLine> lines, std::vector<uint8_t>& data): m_width(width), m_height(height), m_NUM_LINES(NUM_LINES)
{
	m_data = data;
	m_lines = lines;
}

void ComputeShaderPipeline::init()
{
	CreateInstance();
	CreatePhysicalDevice();
	CreateLogicalDevice();
	CreateImage();
	CreateBuffer();
	CreateImageView();
	CreateDescriptorSetLayout();
	CreatePipeline();
	CreateCommandBuffer();
	CreateFence();

	std::cout << "计算管线初始化成功！" << std::endl;
}

void ComputeShaderPipeline::recordCommandBuffer()
{
	vk::DeviceSize storageStagingSize = static_cast<vk::DeviceSize>(m_lines.size() * sizeof(CLine));
	vk::raii::Buffer storageStagingBuffer({});
	vk::raii::DeviceMemory storageStagingMemory({});
	CreateStagingBuffer(storageStagingBuffer, storageStagingMemory, vk::BufferUsageFlagBits::eTransferSrc, storageStagingSize);
	
	void* BufferData = storageStagingMemory.mapMemory(0, storageStagingSize);
	memcpy(BufferData, m_lines.data(), storageStagingSize);
	if (BufferData == nullptr) {
		std::cout << "BufferData is nullptr" << std::endl;
		return;
	}

	
	storageStagingMemory.unmapMemory();
	copyData(storageStagingBuffer, buffer, storageStagingSize);
	
	tansfertionImageLayout(
		image,
		vk::AccessFlagBits::eNone,
		vk::AccessFlagBits::eShaderWrite,
		vk::ImageLayout::eUndefined,
		vk::ImageLayout::eGeneral,
		vk::PipelineStageFlagBits::eTopOfPipe,
		vk::PipelineStageFlagBits::eComputeShader);

	vk::ArrayProxy<const int> value = { m_NUM_LINES };
	commandBuffers[0].reset();
	commandBuffers[0].begin({});
	commandBuffers[0].bindPipeline(vk::PipelineBindPoint::eCompute, computePipeline);
	commandBuffers[0].bindDescriptorSets(vk::PipelineBindPoint::eCompute, layout, 0, { descriptorSet }, {});
	commandBuffers[0].pushConstants(*layout, vk::ShaderStageFlagBits::eCompute, static_cast<uint32_t>(0), value);
	commandBuffers[0].dispatch(256, 1, 1);
	commandBuffers[0].end();

	vk::SubmitInfo submit = {};
	submit.waitSemaphoreCount = 0;
	submit.pWaitSemaphores = nullptr;
	submit.pWaitDstStageMask = nullptr;
	submit.commandBufferCount = 1;
	submit.pCommandBuffers = &*commandBuffers[0];
	submit.signalSemaphoreCount = 0;
	submit.pSignalSemaphores = nullptr;
	submit.pNext = nullptr;

	logicalDevice.resetFences({ fence });
	ComputeQueue.submit(submit, fence);
}

void ComputeShaderPipeline::run()
{
	auto result = logicalDevice.waitForFences({ fence }, VK_TRUE, UINT64_MAX);
	assert(result == vk::Result::eSuccess && "wait for fence fialed");

	logicalDevice.resetFences({ fence });

	tansfertionImageLayout(
		image,
		vk::AccessFlagBits::eShaderWrite,
		vk::AccessFlagBits::eTransferRead,
		vk::ImageLayout::eGeneral,
		vk::ImageLayout::eTransferSrcOptimal,
		vk::PipelineStageFlagBits::eComputeShader,
		vk::PipelineStageFlagBits::eTransfer
	);

	vk::DeviceSize storageImageSize = static_cast<vk::DeviceSize>(m_width * m_height * 4);
	vk::raii::Buffer storageImageBuffer({});
	vk::raii::DeviceMemory storageImageMemory({});
	CreateStagingBuffer(storageImageBuffer, storageImageMemory, vk::BufferUsageFlagBits::eTransferDst, storageImageSize);
	copyImageData(image, storageImageBuffer, vk::ImageLayout::eTransferSrcOptimal);
	
	void* imageData = storageImageMemory.mapMemory(0, storageImageSize);
	if (!imageData) {
		std::cerr << "mapMemory failed!" << std::endl;
		// 直接退出，避免后续访问空指针
		return;
	}
	
	memcpy(m_data.data(), imageData, storageImageSize);
	storageImageMemory.unmapMemory();

}

void ComputeShaderPipeline::CreateStagingBuffer(vk::raii::Buffer& buffer, vk::raii::DeviceMemory& memory, vk::BufferUsageFlags bufferUsage, int BufferSize)
{
	vk::DeviceSize size = static_cast<vk::DeviceSize>(BufferSize);
	vk::BufferCreateInfo stagingBuffer = {};
	stagingBuffer.flags = {};
	stagingBuffer.size = size;
	stagingBuffer.usage = bufferUsage;
	stagingBuffer.sharingMode = vk::SharingMode::eExclusive;
	stagingBuffer.queueFamilyIndexCount = 0;
	stagingBuffer.pQueueFamilyIndices = nullptr;
	stagingBuffer.pNext = nullptr;

	buffer = vk::raii::Buffer(logicalDevice, stagingBuffer);

	vk::MemoryRequirements required = buffer.getMemoryRequirements();
	vk::MemoryAllocateInfo allocate = {};
	allocate.allocationSize = required.size;
	allocate.memoryTypeIndex = findMemoryType(required.memoryTypeBits, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
	allocate.pNext = nullptr;

	memory = vk::raii::DeviceMemory(logicalDevice, allocate);
	buffer.bindMemory(memory, 0);
}

void ComputeShaderPipeline::copyData(vk::raii::Buffer& srcBuffer, vk::raii::Buffer& dstBuffer, vk::DeviceSize size)
{
	vk::CommandBufferBeginInfo beginInfo = {};
	beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
	beginInfo.pInheritanceInfo = nullptr;
	beginInfo.pNext = nullptr;

	commandBuffers[0].reset();
	commandBuffers[0].begin(beginInfo);
	commandBuffers[0].copyBuffer(srcBuffer, dstBuffer, vk::BufferCopy(0, 0, size));
	commandBuffers[0].end();
	
	vk::SubmitInfo submit = {};
	submit.waitSemaphoreCount = 0;
	submit.pWaitSemaphores = nullptr;
	submit.pWaitDstStageMask = nullptr;
	submit.commandBufferCount = 1;
	submit.pCommandBuffers = &*commandBuffers[0];
	submit.signalSemaphoreCount = 0;
	submit.pSignalSemaphores = nullptr;
	submit.pNext = nullptr;

	ComputeQueue.submit(submit);
	ComputeQueue.waitIdle();

	std::cout << "CPU memcpy to GPU complete" << std::endl;
}

void ComputeShaderPipeline::copyImageData(vk::raii::Image& srcImage, vk::raii::Buffer& dstBuffer, vk::ImageLayout layout)
{
	vk::CommandBufferBeginInfo beginInfo = {};
	beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
	beginInfo.pInheritanceInfo = nullptr;
	beginInfo.pNext = nullptr;

	vk::Offset3D tempOffset = { tempOffset.x = 0, tempOffset.y = 0, tempOffset.z = 0 };
	vk::Extent3D tempExtent = { tempExtent.width = m_width, tempExtent.height = m_height, tempExtent.depth = 1 };
	vk::BufferImageCopy bufferImagCopy = {};
	bufferImagCopy.bufferOffset = 0;
	bufferImagCopy.bufferRowLength = 0;
	bufferImagCopy.bufferImageHeight = 0;
	bufferImagCopy.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
	bufferImagCopy.imageSubresource.layerCount = 1;
	bufferImagCopy.imageSubresource.baseArrayLayer = 0;
	bufferImagCopy.imageSubresource.mipLevel = 0;
	bufferImagCopy.imageOffset = tempOffset;
	bufferImagCopy.imageExtent = tempExtent;

	commandBuffers[0].reset();
	commandBuffers[0].begin(beginInfo);
	commandBuffers[0].copyImageToBuffer(srcImage, layout, dstBuffer, bufferImagCopy);
	commandBuffers[0].end();
	
	vk::SubmitInfo submit = {};
	submit.waitSemaphoreCount = 0;
	submit.pWaitSemaphores = nullptr;
	submit.pWaitDstStageMask = nullptr;
	submit.commandBufferCount = 1;
	submit.pCommandBuffers = &*commandBuffers[0];
	submit.signalSemaphoreCount = 0;
	submit.pSignalSemaphores = nullptr;
	submit.pNext = nullptr;

	ComputeQueue.submit(submit, nullptr);
	ComputeQueue.waitIdle();

	std::cout << "GPU memcpy to CPU complete" << std::endl;
}

void ComputeShaderPipeline::CreateInstance()
{
	appInfo.pApplicationName   = appName.data();
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 4, 0);
	appInfo.pEngineName        = engineName.data();
	appInfo.engineVersion      = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion         = VK_API_VERSION_1_4;
	

	vk::InstanceCreateInfo instanceInfo;
	instanceInfo.flags = {};
	instanceInfo.pApplicationInfo        = &appInfo;
	instanceInfo.enabledLayerCount       = requiredLayers.size();
	instanceInfo.ppEnabledLayerNames     = requiredLayers.data();
	instanceInfo.enabledExtensionCount   = requiredExtension.size();
	instanceInfo.ppEnabledExtensionNames = requiredExtension.data();
	instanceInfo.pNext                   = nullptr;

	instance = vk::raii::Instance(context, instanceInfo);
	if (instance == nullptr) {
		std::cout << "instance create failed" << std::endl;
	}
	else
	{
		std::cout << "instance create success" << std::endl;
	}

	setupDebugMessage();
}

void ComputeShaderPipeline::CreatePhysicalDevice() 
{
	std::vector<vk::raii::PhysicalDevice> physical = instance.enumeratePhysicalDevices();

	physicalDevice = physical.at(0);
	if (physicalDevice == nullptr) {
		std::cout << "physical device create failed" << std::endl;
	}
	else
	{
		std::cout << "physical device create success" << std::endl;
	}
}

void ComputeShaderPipeline::CreateLogicalDevice()
{
	CreateQueueFamily();

	vk::DeviceCreateInfo logicalDeviceInfo;
	logicalDeviceInfo.flags                   = {};
	logicalDeviceInfo.queueCreateInfoCount    = 1;
	logicalDeviceInfo.pQueueCreateInfos       = &queueInfo;
	logicalDeviceInfo.enabledLayerCount       = 0;
	logicalDeviceInfo.ppEnabledLayerNames     = nullptr;
	logicalDeviceInfo.enabledExtensionCount   = 0;
	logicalDeviceInfo.ppEnabledExtensionNames = Extensions.data();
	logicalDeviceInfo.pEnabledFeatures        = nullptr;
	logicalDeviceInfo.pNext                   = nullptr;
	
	logicalDevice = vk::raii::Device(physicalDevice, logicalDeviceInfo);
	ComputeQueue = vk::raii::Queue(logicalDevice, 0, 0);
	if (logicalDevice == nullptr) {
		std::cout << "logical device create failed" << std::endl;
	}
	else
	{
		std::cout << "logical device create success" << std::endl;
	}
}

void ComputeShaderPipeline::CreateQueueFamily()
{
	queueInfo.flags            = {};
	queueInfo.queueFamilyIndex = 0;
	queueInfo.queueCount       = 1;
	queueInfo.pQueuePriorities = &priority;
	queueInfo.pNext            = nullptr;
}

void ComputeShaderPipeline::CreateBuffer()
{
	vk::DeviceSize bufferSize = static_cast<vk::DeviceSize>(sizeof(CLine) * m_lines.size());
	vk::BufferCreateInfo bufferInfo = {};
	bufferInfo.flags = {};
	bufferInfo.size = bufferSize;
	bufferInfo.usage = vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eTransferSrc;
	bufferInfo.sharingMode = vk::SharingMode::eExclusive;
	bufferInfo.queueFamilyIndexCount = 0;
	bufferInfo.pQueueFamilyIndices = nullptr;
	bufferInfo.pNext = nullptr;

	buffer = std::move(vk::raii::Buffer(logicalDevice, bufferInfo));

	vk::MemoryRequirements memoryRequired = buffer.getMemoryRequirements();
	vk::MemoryAllocateInfo allocateInfo = {};
	allocateInfo.allocationSize = memoryRequired.size;
	allocateInfo.memoryTypeIndex = findMemoryType(memoryRequired.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);
	allocateInfo.pNext = nullptr;

	memory = std::move(vk::raii::DeviceMemory(logicalDevice, allocateInfo));
	buffer.bindMemory(memory, 0);

	assert(*buffer && "buffer is null");
}

void ComputeShaderPipeline::CreateDescriptorSetLayout()
{
	
	setupPushConstant();
	setupDescriptorSetLayout();
	setupDescriptorPool();
	setupDescriptorSet();
	writeDescriptorSet();
}

void ComputeShaderPipeline::CreateImage()
{
	vk::ImageCreateInfo imageInfo = {};
	imageInfo.flags = {};
	imageInfo.imageType = vk::ImageType::e2D;
	imageInfo.format = vk::Format::eR8G8B8A8Unorm;
	imageInfo.extent = setupImageExtent3D();
	imageInfo.mipLevels = 1;
	imageInfo.arrayLayers = 1;
	imageInfo.samples = vk::SampleCountFlagBits::e1;
	imageInfo.tiling = vk::ImageTiling::eOptimal;
	imageInfo.usage = vk::ImageUsageFlagBits::eStorage | vk::ImageUsageFlagBits::eTransferSrc;
	imageInfo.sharingMode = vk::SharingMode::eExclusive;
	imageInfo.queueFamilyIndexCount = 0;
	imageInfo.pQueueFamilyIndices = nullptr;
	imageInfo.initialLayout = vk::ImageLayout::eUndefined;
	imageInfo.pNext = nullptr;

	image = vk::raii::Image(logicalDevice, imageInfo);

	vk::MemoryRequirements memoryRequired = image.getMemoryRequirements();
	vk::MemoryAllocateInfo allocateInfo = {};
	allocateInfo.allocationSize = memoryRequired.size;
	allocateInfo.memoryTypeIndex = findMemoryType(memoryRequired.memoryTypeBits, vk::MemoryPropertyFlagBits::eDeviceLocal);
	allocateInfo.pNext = nullptr;

	imageMemory = std::move(vk::raii::DeviceMemory(logicalDevice, allocateInfo));
	image.bindMemory(imageMemory, 0);
}

void ComputeShaderPipeline::CreateImageView()
{
	vk::ImageViewCreateInfo viewInfo = {};
	viewInfo.flags = {};
	viewInfo.image = image;
	viewInfo.viewType = vk::ImageViewType::e2D;
	viewInfo.format = vk::Format::eR8G8B8A8Unorm;
	viewInfo.components = setupImageViewComponent();
	viewInfo.subresourceRange = setupImageViewSubresourceRange();
	viewInfo.pNext = nullptr;

	imageView = vk::raii::ImageView(logicalDevice, viewInfo);

	assert(*imageView && "imageView is null");
}

void ComputeShaderPipeline::CreatePipeline()
{
	ComputeShader = CreateShaderMode(ReadFile(compute.data()));

	stageInfo.flags = {};
	stageInfo.stage = vk::ShaderStageFlagBits::eCompute;
	stageInfo.module = ComputeShader;
	stageInfo.pName = "main";
	stageInfo.pSpecializationInfo = nullptr;
	stageInfo.pNext = nullptr;

	layoutInfo.flags = {};
	layoutInfo.setLayoutCount = 1;
	layoutInfo.pSetLayouts = &*descriptorSetLayout;
	layoutInfo.pushConstantRangeCount = 1;
	layoutInfo.pPushConstantRanges = &pushConstants;
	layoutInfo.pNext = nullptr;

	layout = vk::raii::PipelineLayout(logicalDevice, layoutInfo);

	vk::ComputePipelineCreateInfo pipelineInfo;
	pipelineInfo.flags              = {};
	pipelineInfo.stage              = stageInfo;
	pipelineInfo.layout             = layout;
	pipelineInfo.basePipelineHandle = vk::Pipeline();
	pipelineInfo.basePipelineIndex  = -1;
	pipelineInfo.pNext              = nullptr;


	computePipeline = vk::raii::Pipeline(logicalDevice, nullptr, pipelineInfo);
	if (computePipeline == nullptr) {
		std::cout << "compute pipeline create failed" << std::endl;
	}
	else
	{
		std::cout << "compute pipeline create success" << std::endl;
	}
}

void ComputeShaderPipeline::CreateCommandBuffer()
{
	setupCommandPool();
	setupCommandAllocate();
}

void ComputeShaderPipeline::CreateFence()
{
	vk::FenceCreateInfo fenceInfo = {};
	fenceInfo.flags = vk::FenceCreateFlagBits::eSignaled;
	fenceInfo.pNext = nullptr;

	fence = vk::raii::Fence(logicalDevice, fenceInfo);
}

void ComputeShaderPipeline::setupDebugMessage()
{
	//填充验证层调试信息结构体
	vk::DebugUtilsMessengerCreateInfoEXT debugMessengerInfo = {
		debugMessengerInfo.flags           = {},
		debugMessengerInfo.messageSeverity = setupSeverityFlags(),
		debugMessengerInfo.messageType     = setupTypeFlags(),
		debugMessengerInfo.pfnUserCallback = vk::PFN_DebugUtilsMessengerCallbackEXT(debugCallback),
		debugMessengerInfo.pUserData       = nullptr,
		debugMessengerInfo.pNext           = nullptr,
	};

	//判断验证层调试信息是否创建成功
	ValidationLayer = vk::raii::DebugUtilsMessengerEXT(instance, debugMessengerInfo);
	if (ValidationLayer == nullptr) {
		std::cerr << "ValidatorLayer Create failed!" << std::endl;
		return;
	}
	else
	{
		std::clog << "ValidatorLayer Create success!" << std::endl;
	}
}

vk::DebugUtilsMessageSeverityFlagsEXT ComputeShaderPipeline::setupSeverityFlags()
{
	//配置验证层输出信息的危险等级
	vk::DebugUtilsMessageSeverityFlagsEXT severity(
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eError |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
		vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning
	);

	return severity;
}

vk::DebugUtilsMessageTypeFlagsEXT ComputeShaderPipeline::setupTypeFlags()
{
	//配置验证层输出信息的类型
	vk::DebugUtilsMessageTypeFlagsEXT type(
		vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
		vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
		vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance
	);

	return type;
}

VKAPI_ATTR vk::Bool32 VKAPI_CALL ComputeShaderPipeline::debugCallback(vk::DebugUtilsMessageSeverityFlagsEXT severity, vk::DebugUtilsMessageTypeFlagsEXT type, const vk::DebugUtilsMessengerCallbackDataEXT* callbackData, void*)
{
	if (severity.operator&(vk::DebugUtilsMessageSeverityFlagBitsEXT::eError) != vk::DebugUtilsMessageSeverityFlagsEXT{} ||
		severity.operator&(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning) != vk::DebugUtilsMessageSeverityFlagsEXT{} ||
		severity.operator&(vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo) != vk::DebugUtilsMessageSeverityFlagsEXT{}) {
		std::cerr << "validation layer: type" << to_string(type) << " severity: " << to_string(severity) << " msg: " << callbackData->pMessage << std::endl;
	}

	return vk::False;
}


vk::raii::ShaderModule ComputeShaderPipeline::CreateShaderMode(const std::vector<char>& code) const
{
	vk::ShaderModuleCreateInfo shader = {
		shader.flags    = {},
		shader.codeSize = code.size() * sizeof(char),
		shader.pCode    = reinterpret_cast<const uint32_t*>(code.data()),
		shader.pNext    = nullptr,
	};

	vk::raii::ShaderModule shaderModule(logicalDevice, shader);

	return shaderModule;
}


std::vector<char> ComputeShaderPipeline::ReadFile(const std::string& filename)
{
	std::ifstream ifs(filename, std::ios::ate | std::ios::binary);
	if (!ifs.is_open()) {
		throw std::runtime_error("Failed to open file");
	}

	std::vector<char> buffer(ifs.tellg());
	ifs.seekg(0, std::ios::beg);
	ifs.read(buffer.data(), static_cast<std::streamsize>(buffer.size()));
	ifs.close();

	return buffer;
}

void ComputeShaderPipeline::setupPushConstant()
{
	vk::PushConstantRange pushConstant = {};
	pushConstant.stageFlags = vk::ShaderStageFlagBits::eCompute;
	pushConstant.offset = 0;
	pushConstant.size = sizeof(int);
	pushConstants = vk::PushConstantRange(pushConstant);
}

void ComputeShaderPipeline::setupDescriptorSetLayout()
{
	for (int i = 0; i < descriptorType.size(); i++) {
		vk::DescriptorSetLayoutBinding binding;
		binding.binding = i;
		binding.descriptorType = descriptorType[i];
		binding.descriptorCount = 1;
		binding.stageFlags = vk::ShaderStageFlagBits::eCompute;
		binding.pImmutableSamplers = nullptr;
		bindings.push_back(binding);
	}
	
	vk::DescriptorSetLayoutCreateInfo layoutInfo;
	layoutInfo.flags = {};
	layoutInfo.bindingCount = bindings.size();
	layoutInfo.pBindings = bindings.data();
	layoutInfo.pNext = nullptr;

	descriptorSetLayout = vk::raii::DescriptorSetLayout(logicalDevice, layoutInfo);
}

void ComputeShaderPipeline::setupDescriptorPool()
{
	for (int i = 0; i < descriptorType.size(); i++) {
		vk::DescriptorPoolSize size = {};
		size.type = descriptorType[i];
		size.descriptorCount = 1;
		poolSizes.push_back(size);
	}
	
	vk::DescriptorPoolCreateInfo pool = {};
	pool.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
	pool.maxSets = 1;
	pool.poolSizeCount = poolSizes.size();
	pool.pPoolSizes = poolSizes.data();
	pool.pNext = nullptr;

	descriptorPool = vk::raii::DescriptorPool(logicalDevice, pool);
}

void ComputeShaderPipeline::setupDescriptorSet()
{
	vk::DescriptorSetAllocateInfo descriptorAllocate = {};
	descriptorAllocate.descriptorPool = descriptorPool;
	descriptorAllocate.descriptorSetCount = 1;
	descriptorAllocate.pSetLayouts = &*descriptorSetLayout;
	descriptorAllocate.pNext = nullptr;

	descriptorSet = std::move(logicalDevice.allocateDescriptorSets(descriptorAllocate).at(0));
}

void ComputeShaderPipeline::writeDescriptorSet()
{
	vk::DescriptorImageInfo imageInfo = {};
	imageInfo.sampler = vk::Sampler();
	imageInfo.imageView = imageView;
	imageInfo.imageLayout = vk::ImageLayout::eGeneral;

	vk::DescriptorBufferInfo bufferInfo = {};
	bufferInfo.buffer = buffer;
	bufferInfo.offset = 0;
	bufferInfo.range = sizeof(CLine) * m_lines.size();

	vk::WriteDescriptorSet write1 = {};
	write1.dstSet = *descriptorSet;
	write1.dstBinding = 0;
	write1.dstArrayElement = 0;
	write1.descriptorCount = 1;
	write1.descriptorType = vk::DescriptorType::eStorageImage;
	write1.pImageInfo = &imageInfo;
	write1.pBufferInfo = nullptr;
	write1.pTexelBufferView = nullptr;
	write1.pNext = nullptr;

	vk::WriteDescriptorSet write2 = {};
	write2.dstSet = *descriptorSet;
	write2.dstBinding = 1;
	write2.dstArrayElement = 0;
	write2.descriptorCount = 1;
	write2.descriptorType = vk::DescriptorType::eStorageBuffer;
	write2.pImageInfo = nullptr;
	write2.pBufferInfo = &bufferInfo;
	write2.pTexelBufferView = nullptr;
	write2.pNext = nullptr;

	std::vector<vk::WriteDescriptorSet> writeDescriptorSet = { write1, write2 };

	logicalDevice.updateDescriptorSets(writeDescriptorSet, nullptr);
}

void ComputeShaderPipeline::setupCommandPool()
{
	vk::CommandPoolCreateInfo poolInfo = {};
	poolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
	poolInfo.queueFamilyIndex = 0;
	poolInfo.pNext = nullptr;

	commandPool = vk::raii::CommandPool(logicalDevice, poolInfo);
}

void ComputeShaderPipeline::setupCommandAllocate()
{
	commandBuffers.clear();
	vk::CommandBufferAllocateInfo allocateInfo = {};
	allocateInfo.commandPool = commandPool;
	allocateInfo.level = vk::CommandBufferLevel::ePrimary;
	allocateInfo.commandBufferCount = 1;
	allocateInfo.pNext = nullptr;

	commandBuffers = vk::raii::CommandBuffers(logicalDevice, allocateInfo);
}

vk::Extent3D ComputeShaderPipeline::setupImageExtent3D()
{
	vk::Extent3D extent(m_width, m_height, 1);
	return extent;
}

vk::ComponentMapping ComputeShaderPipeline::setupImageViewComponent()
{
	vk::ComponentMapping component(vk::ComponentSwizzle::eR, vk::ComponentSwizzle::eG, vk::ComponentSwizzle::eB, vk::ComponentSwizzle::eA);
	return component;
}

vk::ImageSubresourceRange ComputeShaderPipeline::setupImageViewSubresourceRange()
{
	vk::ImageSubresourceRange range = {};
	range.aspectMask = vk::ImageAspectFlagBits::eColor;
	range.baseMipLevel = 0;
	range.baseArrayLayer = 0;
	range.levelCount = 1;
	range.layerCount = 1;

	return range;
}

uint32_t ComputeShaderPipeline::findMemoryType(uint32_t typeFilter, vk::MemoryPropertyFlags flags)
{
	vk::PhysicalDeviceMemoryProperties memoryProperties = physicalDevice.getMemoryProperties();
	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++) {
		if (typeFilter & (1 << i) && (memoryProperties.memoryTypes[i].propertyFlags & flags) == flags) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

void ComputeShaderPipeline::tansfertionImageLayout(
	vk::raii::Image& image_,
	vk::AccessFlags srcAccessMask,
	vk::AccessFlags dstAccessMask,
	vk::ImageLayout oldLayout,
	vk::ImageLayout newLayout,
	vk::PipelineStageFlags srcStage,
	vk::PipelineStageFlags dstStage
)
{
	vk::CommandBufferBeginInfo beginInfo = {};
	beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
	beginInfo.pInheritanceInfo = nullptr;
	beginInfo.pNext = nullptr;

	vk::ImageMemoryBarrier imageBarrier = {};
	imageBarrier.srcAccessMask = srcAccessMask;
	imageBarrier.dstAccessMask = dstAccessMask;
	imageBarrier.oldLayout = oldLayout;
	imageBarrier.newLayout = newLayout;
	imageBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageBarrier.image = image_;
	imageBarrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
	imageBarrier.subresourceRange.baseMipLevel = 0;
	imageBarrier.subresourceRange.levelCount = 1;
	imageBarrier.subresourceRange.baseArrayLayer = 0;
	imageBarrier.subresourceRange.layerCount = 1;

	commandBuffers[0].reset();
	commandBuffers[0].begin(beginInfo);
	commandBuffers[0].pipelineBarrier(srcStage, dstStage, {}, {}, {}, imageBarrier);
	commandBuffers[0].end();

	vk::SubmitInfo submitInfo = {
		submitInfo.waitSemaphoreCount = 0,
		submitInfo.pWaitSemaphores = nullptr,
		submitInfo.pWaitDstStageMask = nullptr,
		submitInfo.commandBufferCount = 1,
		submitInfo.pCommandBuffers = &*commandBuffers[0],
		submitInfo.signalSemaphoreCount = 0,
		submitInfo.pSignalSemaphores = nullptr,
		submitInfo.pNext = nullptr
	};

	ComputeQueue.submit(submitInfo, nullptr);
	ComputeQueue.waitIdle();
}

std::vector<uint8_t> ComputeShaderPipeline::getData()
{
	return m_data;
}
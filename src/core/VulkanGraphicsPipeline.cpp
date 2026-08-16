
#include<core/VulkanGraphicsPipeline.h>
#include<core/VulkanDescription.h>

me::VulkanGraphicsPipeline::VulkanGraphicsPipeline(vk::raii::Device* logicalDevice, vk::SurfaceFormatKHR format, Description* descriptor)
{
	info.logicalDevice = logicalDevice;
	info.surfaceFormat = format;
	info.descriptor = descriptor;
	info.shaders = {};
	me::VulkanDescription::VertexAttributeDescription();
}

me::VulkanGraphicsPipeline::~VulkanGraphicsPipeline()
{
	info.logicalDevice = nullptr;
	info.shaders.clear();
	info.dynamicSupported.clear();
}


void me::VulkanGraphicsPipeline::CreateGraphicsPipeline()
{
	vk::GraphicsPipelineCreateInfo graphicsPipeline = {
		graphicsPipeline.flags = {},
		graphicsPipeline.stageCount = static_cast<uint32_t>(info.shaders.size()),
		graphicsPipeline.pStages = info.shaders.data(),
		graphicsPipeline.pVertexInputState = &info.inputState,
		graphicsPipeline.pInputAssemblyState = &info.assemblyState,
		graphicsPipeline.pTessellationState = nullptr,
		graphicsPipeline.pViewportState = &info.viewportState,
		graphicsPipeline.pRasterizationState = &info.rasterization,
		graphicsPipeline.pMultisampleState = &info.multiSample,
		graphicsPipeline.pDepthStencilState = &info.depthStencil,
		graphicsPipeline.pColorBlendState = &info.colorBlend,
		graphicsPipeline.pDynamicState = &info.dynamicRender,
		graphicsPipeline.layout = info.pipelineLayout,
		graphicsPipeline.renderPass = nullptr,
		graphicsPipeline.subpass = 0,
		graphicsPipeline.basePipelineHandle = VK_NULL_HANDLE,
		graphicsPipeline.basePipelineIndex = -1,
		graphicsPipeline.pNext = &info.rendering,
	};

	info.graphicsPipeline = vk::raii::Pipeline(*info.logicalDevice, nullptr, graphicsPipeline);
	if (info.graphicsPipeline == nullptr) {
		std::cerr << "Vulkan graphics pipeline Create failed!" << std::endl;
		return;
	}
	else {
		std::clog << "Vulkan graphics pipeline Create success!" << std::endl;
	}
}


void me::VulkanGraphicsPipeline::CreateShaderStateInfo(const std::string& filename, vk::ShaderStageFlagBits stage)
{
	ShaderTypeMatch(stage, filename);

	vk::PipelineShaderStageCreateInfo shader = {
		shader.flags               = {},
		shader.stage               = stage,
		shader.module              = stage == vk::ShaderStageFlagBits::eVertex ? info.vertexShader : info.fragmentShader,
		shader.pName               = "main",
		shader.pSpecializationInfo = nullptr,
		shader.pNext               = nullptr,
	};

	info.shaders.push_back(shader);
	std::cout << "Shader create success!" << std::endl;
}


void me::VulkanGraphicsPipeline::CreateVertexInputStateInfo()
{
	vk::PipelineVertexInputStateCreateInfo vertexInput = {
		vertexInput.flags                           = {},
		vertexInput.vertexBindingDescriptionCount   = static_cast<uint32_t>(me::VulkanDescription::descriptions.size()),
		vertexInput.pVertexBindingDescriptions      = me::VulkanDescription::descriptions.data(),
		vertexInput.vertexAttributeDescriptionCount = static_cast<uint32_t>(me::VulkanDescription::attributes.size()),
		vertexInput.pVertexAttributeDescriptions    = me::VulkanDescription::attributes.data(),
		vertexInput.pNext = nullptr,
	};

	info.inputState = vertexInput;
	std::cout << "Vertex input setup success!" << std::endl;
}


void me::VulkanGraphicsPipeline::CreateInputAssemblyStateInfo(vk::PrimitiveTopology topology)
{
	vk::PipelineInputAssemblyStateCreateInfo assembly = {
		assembly.flags = {},
		assembly.topology = topology,
		assembly.primitiveRestartEnable = VK_FALSE,
		assembly.pNext = nullptr,
	};

	info.assemblyState = assembly;
	std::cout << "Assembly setup success!" << std::endl;
}

void me::VulkanGraphicsPipeline::CreateViewportStateInfo()
{
	vk::PipelineViewportStateCreateInfo viewport = {
		viewport.flags = {},
		viewport.viewportCount = 1,
		viewport.pViewports = nullptr,
		viewport.scissorCount = 1,
		viewport.pScissors = nullptr,
		viewport.pNext = nullptr
	};

	info.viewportState = viewport;
	std::cout << "Viewport setup success!" << std::endl;
}


void me::VulkanGraphicsPipeline::CreateRasterizationStateInfo(
	vk::PolygonMode mode,
	vk::CullModeFlagBits flag,
	vk::FrontFace type
)
{
	vk::PipelineRasterizationStateCreateInfo rasterization = {
		rasterization.flags = {},
		rasterization.depthClampEnable = VK_FALSE,
		rasterization.rasterizerDiscardEnable = VK_FALSE,
		rasterization.polygonMode = mode,
		rasterization.cullMode = flag,
		rasterization.frontFace = type,
		rasterization.depthBiasEnable = VK_FALSE,
		rasterization.depthBiasClamp = 0.0f,
		rasterization.depthBiasConstantFactor = 0.0f,
		rasterization.depthBiasSlopeFactor = 1.0f,
		rasterization.lineWidth = 1.0f,
		rasterization.pNext = nullptr,
	};

	info.rasterization = rasterization;
	std::cout << "Rasterization setup success!" << std::endl;
}


void me::VulkanGraphicsPipeline::CreateMultisampleStateInfo(vk::SampleCountFlagBits flags)
{
	vk::PipelineMultisampleStateCreateInfo multiSample = {
		multiSample.flags = {},
		multiSample.rasterizationSamples = flags,
		multiSample.sampleShadingEnable = VK_FALSE,
		multiSample.minSampleShading = 0.0f,
		multiSample.pSampleMask = nullptr,
		multiSample.alphaToCoverageEnable = VK_FALSE,
		multiSample.alphaToOneEnable = VK_FALSE,
		multiSample.pNext = nullptr,
	};

	info.multiSample = multiSample;
	std::cout << "multiSample setup success!" << std::endl;
}


void me::VulkanGraphicsPipeline::CreateDepthStencilStateInfo()
{
	auto Front = SetupStencilFront();
	auto Back = SetupStencilBack();

	vk::PipelineDepthStencilStateCreateInfo depthStencil = {
		depthStencil.flags = {},
		depthStencil.depthTestEnable = VK_TRUE,
		depthStencil.depthWriteEnable = VK_TRUE,
		depthStencil.depthCompareOp = vk::CompareOp::eLess,
		depthStencil.depthBoundsTestEnable = VK_FALSE,
		depthStencil.stencilTestEnable = VK_FALSE,
		depthStencil.front = Front,
		depthStencil.back = Back,
		depthStencil.minDepthBounds = 0.0f,
		depthStencil.maxDepthBounds = 0.0f,
		depthStencil.pNext = nullptr,
	};

	info.depthStencil = depthStencil;
	std::cout << "depthStencil setup success!" << std::endl;
}


void me::VulkanGraphicsPipeline::CreateColorBlendStateInfo()
{
	CreateRenderingInfo();
	info.attachments.push_back(CreateSimpleColorBlendAttachmentInfo());

	vk::PipelineColorBlendStateCreateInfo colorBlend = {
		colorBlend.flags = {},
		colorBlend.logicOpEnable = VK_FALSE,
		colorBlend.logicOp = vk::LogicOp::eCopy,
		colorBlend.attachmentCount = info.rendering.colorAttachmentCount,
		colorBlend.pAttachments = info.attachments.data(), //注意：这里不能用局部变量地址，会导致悬垂指针，最终影响渲染效果（踩坑记录：2026.03.19）
		colorBlend.blendConstants = {},
		colorBlend.pNext = nullptr,
	};

	info.colorBlend = colorBlend;
	std::cout << "colorBlend setup success!" << std::endl;
}


void me::VulkanGraphicsPipeline::CreateDynamicRenderingStateInfo()
{
	vk::PipelineDynamicStateCreateInfo dynamicRender = {
		dynamicRender.flags = {},
		dynamicRender.dynamicStateCount = static_cast<uint32_t>(info.dynamicSupported.size()),
		dynamicRender.pDynamicStates = info.dynamicSupported.data(),
		dynamicRender.pNext = nullptr,
	};

	info.dynamicRender = dynamicRender;
	std::cout << "dynamicRender setup success!" << std::endl;
}


void me::VulkanGraphicsPipeline::CreatePipelineLayoutStateInfo()
{
	vk::PipelineLayoutCreateInfo layout = {
		layout.flags = {},
		layout.setLayoutCount = static_cast<uint32_t>(info.descriptor->info.layout.size()),
		layout.pSetLayouts = &**info.descriptor->info.layout.data(),
		layout.pushConstantRangeCount = 0,
		layout.pPushConstantRanges = nullptr,
		layout.pNext = nullptr,
	};

	info.pipelineLayout = vk::raii::PipelineLayout(*info.logicalDevice, layout);
	std::cout << "pipelineLayout setup success!" << std::endl;
}


vk::raii::ShaderModule me::VulkanGraphicsPipeline::CreateShaderMode(const std::vector<char>& code) const
{
	vk::ShaderModuleCreateInfo shader = {
		shader.flags    = {},
		shader.codeSize = code.size() * sizeof(char),
		shader.pCode    = reinterpret_cast<const uint32_t*>(code.data()),
		shader.pNext    = nullptr,
	};

	vk::raii::ShaderModule shaderModule(*info.logicalDevice, shader);

	return shaderModule;
}


std::vector<char> me::VulkanGraphicsPipeline::ReadFile(const std::string& filename)
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


vk::StencilOpState me::VulkanGraphicsPipeline::SetupStencilFront()
{
	vk::StencilOpState state = {
		state.failOp = vk::StencilOp::eKeep,
		state.passOp = vk::StencilOp::eKeep,
		state.depthFailOp = vk::StencilOp::eKeep,
		state.compareOp = vk::CompareOp::eNever,
		state.compareMask = 0,
		state.writeMask = 0,
		state.reference = 0,
	};

	return state;
}


vk::StencilOpState me::VulkanGraphicsPipeline::SetupStencilBack()
{
	vk::StencilOpState state = {
		state.failOp = vk::StencilOp::eKeep,
		state.passOp = vk::StencilOp::eKeep,
		state.depthFailOp = vk::StencilOp::eKeep,
		state.compareOp = vk::CompareOp::eNever,
		state.compareMask = 0,
		state.writeMask = 0,
		state.reference = 0,
	};

	return state;
}


void me::VulkanGraphicsPipeline::CreateRenderingInfo()
{
	vk::PipelineRenderingCreateInfo rendering = {
		rendering.viewMask = 0,
		rendering.colorAttachmentCount = 1,
		rendering.pColorAttachmentFormats = &info.surfaceFormat.format,
		rendering.depthAttachmentFormat = vk::Format::eD32Sfloat,
		rendering.stencilAttachmentFormat = vk::Format::eUndefined,
		rendering.pNext = nullptr,
	};

	info.rendering = rendering;
}


vk::PipelineColorBlendAttachmentState me::VulkanGraphicsPipeline::CreateSimpleColorBlendAttachmentInfo()
{
	vk::PipelineColorBlendAttachmentState colorBlendAttachment = {
		colorBlendAttachment.blendEnable = VK_FALSE,
		colorBlendAttachment.srcColorBlendFactor = vk::BlendFactor::eZero,
		colorBlendAttachment.dstColorBlendFactor = vk::BlendFactor::eZero,
		colorBlendAttachment.colorBlendOp = vk::BlendOp::eAdd,
		colorBlendAttachment.srcAlphaBlendFactor = vk::BlendFactor::eZero,
		colorBlendAttachment.dstAlphaBlendFactor = vk::BlendFactor::eZero,
		colorBlendAttachment.alphaBlendOp = vk::BlendOp::eAdd,
		colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR |
											  vk::ColorComponentFlagBits::eG |
											  vk::ColorComponentFlagBits::eB |
											  vk::ColorComponentFlagBits::eA
	};

	return colorBlendAttachment;
}

void me::VulkanGraphicsPipeline::ShaderTypeMatch(vk::ShaderStageFlagBits flags, const std::string& filename)
{
	switch (flags)
	{
	case vk::ShaderStageFlagBits::eVertex:
		info.vertexShader = CreateShaderMode(ReadFile(filename));
		break;
	case vk::ShaderStageFlagBits::eFragment:
		info.fragmentShader = CreateShaderMode(ReadFile(filename));
		break;
	default:
		std::cout << "not have shaderType" << std::endl;
		break;
	}
}
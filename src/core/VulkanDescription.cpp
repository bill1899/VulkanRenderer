
#include<core/VulkanDescription.h>
#include<core/VulkanCommandBuffer.h>

vk::VertexInputBindingDescription me::VulkanDescription::BindVertexDescription(uint32_t bindLocation, vk::VertexInputRate rate, uint32_t stride)
{
	vk::VertexInputBindingDescription description = {
		description.binding = bindLocation,
		description.stride = stride,
		description.inputRate = rate,
	};
	descriptions.push_back(description);

	return description;
}


std::array<vk::VertexInputAttributeDescription, 2> me::VulkanDescription::VertexAttributeDescription()
{
	std::array<vk::VertexInputAttributeDescription, 2> descriptionArray = {
		vk::VertexInputAttributeDescription(0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, position)),
		vk::VertexInputAttributeDescription(1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex,color)),
	};
	attributes = descriptionArray;

	return descriptionArray;
}



me::Description::Description(vk::raii::Device* logicalDevice)
{
	info.logicalDevice = logicalDevice;
}

me::Description::~Description()
{
	info.logicalDevice = nullptr;
	info.sets.clear();
	info.bindings.clear();
	info.sizes.clear();
}


void me::Description::CreateUniformDescription(
	uint32_t bindingLocation,
	vk::DescriptorType type,
	uint32_t descriptorCount,
	vk::ShaderStageFlags flag,
	vk::Sampler* ImmutableSamplers
)
{
	vk::DescriptorSetLayoutBinding binding = {};
	binding.binding = bindingLocation;
	binding.descriptorType = type;
	binding.descriptorCount = descriptorCount;
	binding.stageFlags = flag;
	binding.pImmutableSamplers = ImmutableSamplers;

	info.bindings.push_back(binding);

	vk::DescriptorSetLayoutCreateInfo layout = {};
	layout.flags = {};
	layout.bindingCount = static_cast<uint32_t>(info.bindings.size());
	layout.pBindings = info.bindings.data();
	layout.pNext = nullptr;

	info.layout.emplace_back(vk::raii::DescriptorSetLayout(*info.logicalDevice, layout));

	vk::DescriptorPoolSize size = {};
	size.type = vk::DescriptorType::eUniformBuffer;
	size.descriptorCount = MAX_FRAME_FIGHT;

	vk::DescriptorPoolCreateInfo pool = {};
	pool.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
	pool.maxSets = MAX_FRAME_FIGHT;
	pool.poolSizeCount = static_cast<uint32_t>(info.sizes.size());
	pool.pPoolSizes = info.sizes.data();
	pool.pNext = nullptr;

	info.pools.emplace_back(vk::raii::DescriptorPool(*info.logicalDevice, pool));

	vk::DescriptorSetAllocateInfo allocate = {};
	allocate.descriptorPool = *info.pools.data();
	allocate.descriptorSetCount = 1;
	allocate.pSetLayouts = &**info.layout.data();

	info.sets.clear();
	info.sets = info.logicalDevice->allocateDescriptorSets(allocate);

	vk::DescriptorBufferInfo buffer = {};
	vk::WriteDescriptorSet write = {};

	info.logicalDevice->updateDescriptorSets(write, nullptr);
}
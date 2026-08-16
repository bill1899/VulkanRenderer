//Vulkan描述符系统

#pragma once
#include<iostream>
#include<array>
#include<glm/glm.hpp>
#include<glm/gtc/matrix_transform.hpp>
#include<vulkan/vulkan_raii.hpp>

namespace me {

	struct Vertex {
		glm::vec3 position;
		glm::vec3 color;
	};

	struct VulkanDescription
	{
		inline static std::vector<vk::VertexInputBindingDescription> descriptions;

		inline static std::array<vk::VertexInputAttributeDescription, 2> attributes;

		//顶点输入布局绑定描述符
		static vk::VertexInputBindingDescription BindVertexDescription(uint32_t bindLocation, vk::VertexInputRate rate, uint32_t stride);

		//顶点输入属性描述符
		static std::array<vk::VertexInputAttributeDescription, 2> VertexAttributeDescription();
	};

	struct DescriptionInfo {
		std::vector<vk::raii::DescriptorSetLayout> layout;  //描述符布局容器
		std::vector<vk::raii::DescriptorPool> pools;        //描述符池容器
		vk::raii::Device* logicalDevice = nullptr;          //逻辑设备view指针
		std::vector<vk::raii::DescriptorSet> sets;          //描述符集容器
		std::vector<vk::DescriptorSetLayoutBinding> bindings;//描述符集布局绑定容器
		std::vector<vk::DescriptorPoolSize> sizes;           //描述符池大小容器
	};

	//描述符系统
	class Description
	{
	public:
		DescriptionInfo info;

	public:
		Description(vk::raii::Device* logicalDevice);
		~Description();

		void CreateUniformDescription(
			uint32_t bindingLocation,
			vk::DescriptorType type,
			uint32_t descriptorCount,
			vk::ShaderStageFlags flag,
			vk::Sampler* ImmutableSamplers = {}
		);

	private:

	};
}
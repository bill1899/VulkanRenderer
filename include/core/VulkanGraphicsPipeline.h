//Vulkan图形管线类

#pragma once
#include<iostream>
#include<string>
#include<fstream>
#include<vulkan/vulkan_raii.hpp>

namespace me {

	class VulkanDescription;
	class Description;

	//Vulkan图形管线属性结构体
	struct VulkanGraphicsPipelineInfo {
		vk::raii::Pipeline graphicsPipeline = nullptr; //Vulkan图形管线类
		vk::raii::Device*   logicalDevice   = nullptr; //逻辑设备
		vk::raii::PipelineLayout pipelineLayout = nullptr; //管线布局
		vk::SurfaceFormatKHR surfaceFormat;
		vk::raii::ShaderModule vertexShader = nullptr;  //顶点着色器
		vk::raii::ShaderModule fragmentShader = nullptr; //片段着色器
		me::Description* descriptor;


		vk::PipelineVertexInputStateCreateInfo inputState;       //顶点输入阶段
		vk::PipelineInputAssemblyStateCreateInfo assemblyState;  //图元装配阶段
		vk::PipelineViewportStateCreateInfo viewportState;       //视图裁剪阶段
		vk::PipelineRasterizationStateCreateInfo rasterization;  //光栅化阶段
		vk::PipelineMultisampleStateCreateInfo multiSample;      //多重采样阶段
		vk::PipelineDepthStencilStateCreateInfo depthStencil;    //深度模板测试阶段
		vk::PipelineRenderingCreateInfo rendering;               //动态渲染信息
		vk::PipelineColorBlendStateCreateInfo colorBlend;        //颜色混合阶段
		vk::PipelineDynamicStateCreateInfo dynamicRender;        //动态渲染阶段


		std::vector<vk::PipelineShaderStageCreateInfo> shaders;
		std::vector<vk::PipelineColorBlendAttachmentState> attachments;
		std::vector<vk::ShaderModule> modules;
		std::vector<vk::DynamicState> dynamicSupported = {       //支持动态渲染的阶段
			vk::DynamicState::eViewport,
			vk::DynamicState::eScissor,
		}; 
	};

	class VulkanGraphicsPipeline
	{
	public:
		VulkanGraphicsPipelineInfo info;

	public:
		VulkanGraphicsPipeline(vk::raii::Device* logicalDevice, vk::SurfaceFormatKHR format, Description* descriptor);
		~VulkanGraphicsPipeline();

		//创建图形管线
		void CreateGraphicsPipeline();

		//着色器阶段
		void CreateShaderStateInfo(const std::string& filename, vk::ShaderStageFlagBits stage);

		//顶点输入阶段
		void CreateVertexInputStateInfo();

		//图元装配阶段
		void CreateInputAssemblyStateInfo(vk::PrimitiveTopology topology);

		//裁剪视图阶段
		void CreateViewportStateInfo();

		//光栅化阶段
		void CreateRasterizationStateInfo(vk::PolygonMode mode, vk::CullModeFlagBits flag, vk::FrontFace type);

		//多重采样阶段
		void CreateMultisampleStateInfo(vk::SampleCountFlagBits flags);

		//深度测试阶段
		void CreateDepthStencilStateInfo();

		//颜色混合阶段
		void CreateColorBlendStateInfo();

		//动态渲染阶段
		void CreateDynamicRenderingStateInfo();

		//管线布局阶段
		void CreatePipelineLayoutStateInfo();

	private:
		//创建着色器模块
		vk::raii::ShaderModule CreateShaderMode(const std::vector<char>& code) const;

		//读取着色器源文件
		std::vector<char> ReadFile(const std::string& filename);

		//设置模板测试
		vk::StencilOpState SetupStencilFront();
		vk::StencilOpState SetupStencilBack();

		//创建渲染信息
		void CreateRenderingInfo();

		//创建单一颜色混合附件
		vk::PipelineColorBlendAttachmentState CreateSimpleColorBlendAttachmentInfo();

		//着色器匹配
		void ShaderTypeMatch(vk::ShaderStageFlagBits flags, const std::string& filename);
	};
}

#include<core/VulkanInstance.h>

me::VulkanInstance::VulkanInstance(
	std::string applicationName,
	uint32_t    applicationVersion,
	std::string engineName,
	uint32_t    engineVersion,
	uint32_t    apiVersion,
	const void* pNext
)
{
	CreateInstanceProperty(applicationName, applicationVersion, engineName, engineVersion, apiVersion, pNext);
	CheckRequiredLayers();
	CheckRequiredExtensions();
	CreateVulkanInstance();
}

me::VulkanInstance::~VulkanInstance()
{

}


void me::VulkanInstance::CreateVulkanInstance()
{
	vk::InstanceCreateInfo instanceInfo = {
		instanceInfo.flags                   = {},
		instanceInfo.pApplicationInfo        = &info.app,
		instanceInfo.enabledLayerCount       = static_cast<uint32_t>(info.requiredLayers.size()),
		instanceInfo.ppEnabledLayerNames     = info.requiredLayers.data(),
		instanceInfo.enabledExtensionCount   = static_cast<uint32_t>(info.requiredExtensions.size()),
		instanceInfo.ppEnabledExtensionNames = info.requiredExtensions.data(),
		instanceInfo.pNext                   = nullptr,
	};

	info.instance = vk::raii::Instance(info.context, instanceInfo);
	if (info.instance == nullptr) {
		std::cerr << "Vulkan instance create failed" << std::endl;
		return;
	}
	else
	{
		std::clog << "Vulkan instance create success" << std::endl;
	}
}

void me::VulkanInstance::CreateInstanceProperty(
	std::string applicationName,
	uint32_t    applicationVersion,
	std::string engineName,
	uint32_t    engineVersion,
	uint32_t    apiVersion,
	const void* next
)
{
	vk::ApplicationInfo appInfo = {
		appInfo.pApplicationName   = applicationName.data(),
		appInfo.applicationVersion = applicationVersion,
		appInfo.pEngineName        = engineName.data(),
		appInfo.engineVersion      = engineVersion,
		appInfo.apiVersion         = apiVersion,
		appInfo.pNext              = next
	};

	info.app = appInfo;
}



void me::VulkanInstance::CheckRequiredLayers()
{
	std::vector<const char*> requiredLayers;
	if (enableValidatorLayers) {
		requiredLayers.assign(info.requiredValidation.begin(), info.requiredValidation.end());
	}

	auto layerProperties = info.context.enumerateInstanceLayerProperties();
	for (const auto& requiredLayer : requiredLayers) {
		if (std::ranges::none_of(layerProperties,
			[requiredLayer](const auto& layerProperty) {
				return strcmp(layerProperty.layerName, requiredLayer) == 0;
			}
		)) {
			throw std::runtime_error("required layers not supported:" + static_cast<std::string>(requiredLayer));
		}
	}

	info.requiredLayers = requiredLayers;
}

void me::VulkanInstance::CheckRequiredExtensions()
{
	std::vector<const char*> requiredExtensions = getRequiredExtensions();

	auto extensionsProperties = info.context.enumerateInstanceExtensionProperties();
	for (const auto& requiredExtension : requiredExtensions) {
		if (std::ranges::none_of(extensionsProperties,
			[requiredExtension](const auto& extensionProperty) {
				return strcmp(extensionProperty.extensionName, requiredExtension) == 0;
			}
		)) {
			throw std::runtime_error("required extensions not supported:" + static_cast<std::string>(requiredExtension));
		}
	}

	info.requiredExtensions = requiredExtensions;
}

std::vector<const char*> me::VulkanInstance::getRequiredExtensions()
{
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);
	if (enableValidatorLayers) {
		extensions.push_back(vk::EXTDebugUtilsExtensionName);
	}

	return extensions;
}

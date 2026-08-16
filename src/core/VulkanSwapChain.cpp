
#include<core/VulkanSwapChain.h>

me::VulkanSwapChain::VulkanSwapChain(
	vk::raii::PhysicalDevice& physicalDevice, 
	vk::raii::Device& logicalDevice, 
	vk::raii::SurfaceKHR& surface,
	GLFWwindow* window
)
{
	CreateSwapChain(physicalDevice, logicalDevice, surface, window);
	CreateImageView(logicalDevice);
}

me::VulkanSwapChain::~VulkanSwapChain()
{

}

void me::VulkanSwapChain::CreateSwapChain(
	vk::raii::PhysicalDevice& physicalDevice, 
	vk::raii::Device& logicalDevice, 
	vk::raii::SurfaceKHR& surface,
	GLFWwindow* window
)
{
	auto surfaceCapabilities = physicalDevice.getSurfaceCapabilitiesKHR(*surface);
	info.swapChainFormat = ChooseSwapChainSurfaceFormat(physicalDevice.getSurfaceFormatsKHR(*surface));
	info.swapChainExtent = ChooseSwapChainExtent(surfaceCapabilities, window);

	vk::SwapchainCreateInfoKHR swapChain = {
		swapChain.flags                 = {},
		swapChain.surface               = *surface,
		swapChain.minImageCount         = ChooseSwapChainImageCount(surfaceCapabilities),
		swapChain.imageFormat           = info.swapChainFormat.format,
		swapChain.imageColorSpace       = info.swapChainFormat.colorSpace,
		swapChain.imageExtent           = info.swapChainExtent,
		swapChain.imageArrayLayers      = 1,
		swapChain.imageUsage            = vk::ImageUsageFlagBits::eColorAttachment,
		swapChain.imageSharingMode      = vk::SharingMode::eExclusive,
		swapChain.queueFamilyIndexCount = 0,
		swapChain.pQueueFamilyIndices   = nullptr,
		swapChain.preTransform          = surfaceCapabilities.currentTransform,
		swapChain.compositeAlpha        = vk::CompositeAlphaFlagBitsKHR::eOpaque,
		swapChain.presentMode           = ChooseSwapChainPresent(physicalDevice.getSurfacePresentModesKHR(*surface)),
		swapChain.clipped               = VK_TRUE,
		swapChain.oldSwapchain          = nullptr,
		swapChain.pNext                 = nullptr
	};

	info.swapChain = vk::raii::SwapchainKHR(logicalDevice, swapChain);
	info.swapChainImages = info.swapChain.getImages();
	if (info.swapChain == nullptr) {
		std::cerr << "Vulkan swap chain Create failed!" << std::endl;
		return;
	}
	else {
		std::clog << "Vulkan swap chain Create success!" << std::endl;
	}
}

void me::VulkanSwapChain::CreateImageView(vk::raii::Device& logicalDevice)
{
	assert(info.swapChainImageViews.empty() && "ImageView not be empty");
	//info.swapChainImageViews.resize(info.swapChainImages.size());
	for (int i = 0; i < info.swapChainImages.size(); ++i) {
		vk::ImageViewCreateInfo imageView = {
			imageView.flags            = {},
			imageView.image            = info.swapChainImages[i],
			imageView.viewType         = vk::ImageViewType::e2D,
			imageView.format           = info.swapChainFormat.format,
			imageView.components       = ChooseImageViewMapping(),
			imageView.subresourceRange = ChooseImageViewSubresourceRange(),
			imageView.pNext            = nullptr,
		};
		info.swapChainImageViews.push_back(vk::raii::ImageView(logicalDevice, imageView));
	}
	std::clog << "ImageView Create success!" << std::endl;
}

uint32_t me::VulkanSwapChain::ChooseSwapChainImageCount(vk::SurfaceCapabilitiesKHR surfaceCapabilities)
{
	auto minImageCount = std::max(info.maxImage, surfaceCapabilities.minImageCount);
	if (surfaceCapabilities.maxImageCount > 0 && surfaceCapabilities.maxImageCount < minImageCount) {
		minImageCount = surfaceCapabilities.maxImageCount;
	}

	return minImageCount;
}

vk::SurfaceFormatKHR me::VulkanSwapChain::ChooseSwapChainSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& formats)
{
	assert(!formats.empty() && "SwapChain Surface Format not be empty");
	const auto It = std::ranges::find_if(formats,
		[](const auto& format) {
			return format.format == vk::Format::eB8G8R8A8Srgb && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear;
		}
	);
	
	return It != formats.end() ? *It : formats[0];
}


vk::Extent2D me::VulkanSwapChain::ChooseSwapChainExtent(vk::SurfaceCapabilitiesKHR& surfaceCapabilities, GLFWwindow* window)
{
	if (surfaceCapabilities.currentExtent.width != 0xFFFFFFFF) {
		return surfaceCapabilities.currentExtent;
	}

	int width = 0;
	int height = 0;
	glfwGetFramebufferSize(window, &width, &height);

	vk::Extent2D extent = {
		extent.width = std::clamp<uint32_t>(width, surfaceCapabilities.minImageExtent.width, surfaceCapabilities.maxImageExtent.width),
		extent.height = std::clamp<uint32_t>(height, surfaceCapabilities.minImageExtent.height, surfaceCapabilities.maxImageExtent.height)
	};

	return extent;
}

vk::PresentModeKHR me::VulkanSwapChain::ChooseSwapChainPresent(const std::vector<vk::PresentModeKHR>& presents)
{
	assert(std::ranges::any_of(presents,
		[](auto& presentMode) {
			return presentMode == vk::PresentModeKHR::eFifoLatestReady;})
	);

	return std::ranges::any_of(presents,
		[](const vk::PresentModeKHR presentMode) {
			return presentMode == vk::PresentModeKHR::eMailbox;
		}
	) ? vk::PresentModeKHR::eMailbox : vk::PresentModeKHR::eFifo;
}


vk::ComponentMapping me::VulkanSwapChain::ChooseImageViewMapping()
{
	vk::ComponentMapping mapping = {
		mapping.r = vk::ComponentSwizzle::eR,
		mapping.g = vk::ComponentSwizzle::eG,
		mapping.b = vk::ComponentSwizzle::eB,
		mapping.a = vk::ComponentSwizzle::eA,
	};

	return mapping;
}


vk::ImageSubresourceRange me::VulkanSwapChain::ChooseImageViewSubresourceRange()
{
	vk::ImageSubresourceRange range = {
		range.aspectMask = vk::ImageAspectFlagBits::eColor,
		range.baseMipLevel = 0,
		range.levelCount = 1,
		range.baseArrayLayer = 0,
		range.layerCount = 1,
	};

	return range;
}
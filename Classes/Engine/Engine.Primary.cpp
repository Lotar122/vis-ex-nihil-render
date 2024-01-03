#include "Engine.hpp"

void nihil::Engine::CreateVulkanInstance(VulkanInstanceCreateInfo createInfo) {
	uint32_t version{ 0 };
	version &= ~(0xFFFU);
	version = VK_MAKE_API_VERSION(0, 1, 0, 0);
	vkEnumerateInstanceVersion(&version);
	vk::ApplicationInfo appInfo = vk::ApplicationInfo(
		createInfo.appName.c_str(),
		VK_MAKE_API_VERSION(createInfo.appVersion.variant, createInfo.appVersion.major, createInfo.appVersion.minor, createInfo.appVersion.patch),
		"Nihil",
		version,
		VK_MAKE_API_VERSION(createInfo.vulkanVersion.variant, createInfo.vulkanVersion.major, createInfo.vulkanVersion.minor, createInfo.vulkanVersion.patch)
	);
	uint32_t extensionCount;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);

	// Create a vector of const char* from glfwExtensions
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + extensionCount);

	std::vector<const char*> validationLayers;

	if (createInfo.validationLayers) {
		validationLayers = {
			"VK_LAYER_KHRONOS_validation"
		};
	}
	else {
		validationLayers = {};
	}

	vk::InstanceCreateInfo VKCreateInfo = vk::InstanceCreateInfo(
		vk::InstanceCreateFlags(),
		&appInfo,
		static_cast<uint32_t>(validationLayers.size()), validationLayers.data(),
		static_cast<uint32_t>(extensions.size()), extensions.data()
	);

	try {
		instance = vk::createInstance(VKCreateInfo);
	}
	catch (vk::SystemError err) {
		std::abort();
	}
}

void nihil::Engine::PickPhysicalDevice()
{
	bool foundDevice = false;
	std::vector<vk::PhysicalDevice> availableDevices = instance.enumeratePhysicalDevices();
	for (const vk::PhysicalDevice& devices : availableDevices)
	{
		if (isSuitable(devices))
		{
			device = devices;
			foundDevice = true;
		}
	}
	if (!foundDevice) {
		std::abort();
	}
}

void nihil::Engine::CreateVulkanSurface(GLFWwindow* window)
{
	VkSurfaceKHR c_surface;
	glfwCreateWindowSurface(instance, window, NULL, &c_surface);
	surface = c_surface;
}

void nihil::Engine::CreateVulkanQueues()
{
	std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();
	int iterator1 = 0;
	for (const vk::QueueFamilyProperties& queueFamily : queueFamilies)
	{
		/*
		* Graphics_BIT : 0x00000001;
		* Compute_BIT: 0x00000002;
		* Transfer_BIT: 0x00000004;
		* Sparse_Binding_BIT: 0x00000008;
		*/
		if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
		{
			famind.graphics = iterator1;
			famind.present = iterator1;
		}
		if (device.getSurfaceSupportKHR(iterator1, surface))
			if (famind.isComplete())
			{
				break;
			}
		iterator1++;
	}
	uniqueFAMIND.push_back(famind.graphics.value());
	if (famind.graphics.value() != famind.present.value())
	{
		uniqueFAMIND.push_back(famind.present.value());
	}

	//finalize the queue creation in CreateVulkanLogicalDevice
}

void nihil::Engine::CreateVulkanLogicalDevice()
{
	float queuepriority = 1.0f;
	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfo;
	for (uint32_t index : uniqueFAMIND)
	{
		queueCreateInfo.push_back(vk::DeviceQueueCreateInfo(
			vk::DeviceQueueCreateFlags(), famind.graphics.value(), 1,
			&queuepriority
		));
	}

	std::vector<const char*> deviceExtension = {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	vk::PhysicalDeviceFeatures deviceFeatures = vk::PhysicalDeviceFeatures();
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	vk::DeviceCreateInfo deviceInfo = vk::DeviceCreateInfo(
		vk::DeviceCreateFlags(), queueCreateInfo.size(), queueCreateInfo.data(), 0, NULL, deviceExtension.size(), deviceExtension.data(), &deviceFeatures
	);

	try {
		logicalDevice = device.createDevice(deviceInfo);
	}
	catch (vk::SystemError err) {
		std::abort();
	}

	graphicsQueue = logicalDevice.getQueue(famind.graphics.value(), 0);
	presentQueue = logicalDevice.getQueue(famind.present.value(), 0);
}

nihil::SwapchainConfiguration nihil::Engine::CreateSwapchainConfiguration(SwapchainConfigCreateInfo createInfo)
{
	SwapchainSupportDetails support;
	vk::SurfaceFormatKHR surfaceFormat;
	vk::PresentModeKHR presentMode;
	vk::Extent2D extent;

	support.capabilities = device.getSurfaceCapabilitiesKHR(surface);
	support.formats = device.getSurfaceFormatsKHR(surface);
	support.presentModes = device.getSurfacePresentModesKHR(surface);

	bool set1 = false;
	for (const vk::SurfaceFormatKHR& format : support.formats)
	{
		if (format == vk::Format::eB8G8R8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
			surfaceFormat = format;
			set1 = true;
			break;
		}
	}
	if (!set1) { if (support.formats.size() > 0) { surfaceFormat = support.formats[0]; } else { std::abort(); } }

	bool set2 = false;
	for (const vk::PresentModeKHR& pForm : support.presentModes)
	{
		if (pForm == vk::PresentModeKHR::eMailbox)
		{
			presentMode = pForm;
			set2 = true;
			break;
		}
	}
	if (!set2) presentMode = vk::PresentModeKHR::eFifo;

	extent.setWidth(createInfo.windowWidth);
	extent.setHeight(createInfo.windowHeight);
	bool set3 = false;
	if (support.capabilities.currentExtent.width != UINT32_MAX) {
		extent = support.capabilities.currentExtent;
		set3 = true;
	}
	else {
		extent.width = std::min(
			(int)support.capabilities.maxImageExtent.width,
			std::max((int)support.capabilities.minImageExtent.width, (int)createInfo.windowWidth)
		);
		extent.height = std::min(
			(int)support.capabilities.maxImageExtent.height,
			std::max((int)support.capabilities.minImageExtent.height, (int)createInfo.windowHeight)
		);
	}

	uint8_t imageCount = std::min(
		support.capabilities.maxImageCount,
		support.capabilities.minImageCount + (int)createInfo.preferredBuffering
	);
	if (imageCount >= 3) {
		imageCount = 3;
	}

	vk::SwapchainCreateInfoKHR swapchainCreateInfo = vk::SwapchainCreateInfoKHR(
		vk::SwapchainCreateFlagsKHR(), surface, imageCount, surfaceFormat.format, surfaceFormat.colorSpace,
		extent, 1, vk::ImageUsageFlagBits::eColorAttachment
	);

	if (famind.graphics.value() != famind.present.value())
	{
		swapchainCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
		swapchainCreateInfo.queueFamilyIndexCount = uniqueFAMIND.size();
		swapchainCreateInfo.pQueueFamilyIndices = uniqueFAMIND.data();
	}
	else
	{
		swapchainCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
	}
	swapchainCreateInfo.preTransform = support.capabilities.currentTransform;
	swapchainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	swapchainCreateInfo.presentMode = presentMode;
	swapchainCreateInfo.clipped = VK_TRUE;
	swapchainCreateInfo.oldSwapchain = vk::SwapchainKHR(nullptr);

	return swapchainCreateInfo;
}

void nihil::Engine::CreateSwapchain(SwapchainConfiguration createInfo)
{
	swapchainConfiguration = createInfo;
	try {
		swapchainBundle.swapchain = logicalDevice.createSwapchainKHR(createInfo);
	}
	catch (vk::SystemError err) {
		std::abort();
	}

	swapchainBundle.format = createInfo.imageFormat;
	swapchainBundle.extent = createInfo.imageExtent;

	std::cout << createInfo.imageExtent.width << createInfo.imageExtent.height << std::endl;
}

void nihil::Engine::CreateImageViews()
{
	std::vector <vk::Image> images = logicalDevice.getSwapchainImagesKHR(swapchainBundle.swapchain);
	swapchainBundle.frames.resize(images.size());
	for (size_t i = 0; i < images.size(); i++)
	{
		vk::ImageViewCreateInfo imageViewCreateInfo = {};
		imageViewCreateInfo.image = images[i];
		imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
		imageViewCreateInfo.components.r = vk::ComponentSwizzle::eIdentity;
		imageViewCreateInfo.components.g = vk::ComponentSwizzle::eIdentity;
		imageViewCreateInfo.components.b = vk::ComponentSwizzle::eIdentity;
		imageViewCreateInfo.components.a = vk::ComponentSwizzle::eIdentity;
		imageViewCreateInfo.format = vk::Format::eB8G8R8A8Unorm;

		imageViewCreateInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		imageViewCreateInfo.subresourceRange.baseMipLevel = 0;
		imageViewCreateInfo.subresourceRange.levelCount = 1;
		imageViewCreateInfo.subresourceRange.baseArrayLayer = 0;
		imageViewCreateInfo.subresourceRange.layerCount = 1;

		swapchainBundle.frames[i].image = images[i];
		swapchainBundle.frames[i].view = logicalDevice.createImageView(imageViewCreateInfo);
	}
	maxFramesInFlight = swapchainBundle.frames.size();
	frameNumber = 0;
}
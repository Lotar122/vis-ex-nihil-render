#include "Engine.h"

void Engine::CreateVulkanInstance(InstanceCreateInfo& info)
{
		//"VK_LAYER_KHRONOS_validation",

	if (debug) std::cout << YELLOW << "[Setup]" << RESET << "Creating a Vulkan instance ";
	uint32_t version{ 0 };
	version &= ~(0xFFFU);
	version = VK_MAKE_API_VERSION(0, 1, 0, 0);
	vkEnumerateInstanceVersion(&version);
	vk::ApplicationInfo appInfo = vk::ApplicationInfo(
		info.AppName.c_str(),
		VK_MAKE_API_VERSION(info.AppVersion.variant, info.AppVersion.major, info.AppVersion.minor, info.AppVersion.patch),
		"Maxwells Egnine",
		version,
		VK_MAKE_API_VERSION(info.VulkanVersion.variant, info.VulkanVersion.major, info.VulkanVersion.minor, info.VulkanVersion.patch)
	);
	uint32_t extensionCount;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);

	// Create a vector of const char* from glfwExtensions
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + extensionCount);

	vk::InstanceCreateInfo createInfo = vk::InstanceCreateInfo(
		vk::InstanceCreateFlags(),
		&appInfo,
		implicit_cast<uint32_t>(info.validationLayers.size()), info.validationLayers.data(),
		implicit_cast<uint32_t>(extensions.size()), extensions.data()
	);

	try {
		instance = vk::createInstance(createInfo);
	}
	catch (vk::SystemError err) {
		std::cerr << RED << "[###]" << RESET << std::endl;
		error = true;
		finishSetup();
		std::abort();
	}
	std::cout << GREEN << "[###]" << RESET << std::endl;
}

void Engine::PickDevice()
{
	bool foundDevice = false;
	if (debug) std::cout << YELLOW << "[Setup]" << RESET << "Picking the device:" << std::endl;
	std::vector<vk::PhysicalDevice> availableDevices = instance.enumeratePhysicalDevices();
	for (const vk::PhysicalDevice& devices : availableDevices)
	{
		std::cout << "Enum" << std::endl;
		if (isSuitable(devices))
		{
			device = devices;
			foundDevice = true;
			std::cout << YELLOW << "[Setup]" << RESET << MAGENTA << "->" << RESET << YELLOW << "[Pick-Device]" << RESET << "found suitable device  "<<MAGENTA<<"--->  " << CYAN << device.getProperties().deviceName << " ";
		}
	}
	if (!foundDevice) {
		std::cerr << YELLOW << "[Setup]" << RESET << MAGENTA << "->" << RESET << YELLOW << "[Pick-Device]" << RESET << "Couldn't find suitable device " << RED << "[###]" << RESET << std::endl;
		error = true;
		finishSetup();
		std::abort();
	}
	else {
		std::cout << GREEN << "[###]" << RESET << std::endl;
	}

	//add multiple GPU handling in future
}

void Engine::CreateSurface()
{
	if (debug) std::cout << YELLOW << "[Setup]" << RESET << "Creating the surface " << GREEN << "[###]" << RESET << std::endl;
	VkSurfaceKHR c_surface;
	glfwCreateWindowSurface(instance, app->window, NULL, &c_surface);
	surface = c_surface;
}

void Engine::CreateQueues()
{
	if (debug) std::cout << YELLOW << "[Setup]" << RESET << "Creating the queues " << GREEN << "[###]" << RESET << std::endl;
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
	uniqueFAMIND;
	uniqueFAMIND.push_back(famind.graphics.value());
	if (famind.graphics.value() != famind.present.value())
	{
		uniqueFAMIND.push_back(famind.present.value());
	}
}

void Engine::CreateLogicDevice()
{
	if (debug) std::cout << YELLOW << "[Setup]" << RESET << "Creating a Vulkan logical Device ";
	float queuepriority = 1.0f;
	//use famind
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
		std::cerr << RED << "[###]" << RESET << std::endl;
		error = true;
		finishSetup();
		std::abort();
	}
	std::cout << GREEN << "[###]" << RESET << std::endl;
}

void Engine::PreConfigSwapchain()
{
	if (debug) std::cout << YELLOW << "[Setup]" << RESET << "Pre-Configuring the swapchain " << GREEN << "[###]" << RESET << std::endl;
	graphicsQueue = logicalDevice.getQueue(famind.graphics.value(), 0);
	presentQueue = logicalDevice.getQueue(famind.present.value(), 0);

	support;
	support.capabilities = device.getSurfaceCapabilitiesKHR(surface);
	support.formats = device.getSurfaceFormatsKHR(surface);
	support.presentModes = device.getSurfacePresentModesKHR(surface);

	surfaceFormat;
	bool set1 = false;
	for (const vk::SurfaceFormatKHR& format : support.formats)
	{
		if (format == vk::Format::eB8G8R8A8Unorm && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
			surfaceFormat = format;
			set1 = true;
			break;
		}
	}
	if (!set1) { if (support.formats.size() > 0) { surfaceFormat = support.formats[0]; } else { std::cerr << "failed to create swapchain no formats specified" << std::endl; } }

	presentMode;
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

	extent.setWidth(implicit_cast<uint32_t>(app->width));
	extent.setHeight(implicit_cast<uint32_t>(app->height));
	bool set3 = false;
	if (support.capabilities.currentExtent.width != UINT32_MAX) {
		extent = support.capabilities.currentExtent;
		set3 = true;
	}
	else {
		extent.width = std::min(
			(int)support.capabilities.maxImageExtent.width,
			std::max((int)support.capabilities.minImageExtent.width, (int)app->width)
		);
		extent.height = std::min(
			(int)support.capabilities.maxImageExtent.height,
			std::max((int)support.capabilities.minImageExtent.height, (int)app->height)
		);
	}

	imageCount = std::min(
		support.capabilities.maxImageCount,
		support.capabilities.minImageCount + 1
	);

	swapCreateInfo = vk::SwapchainCreateInfoKHR(
		vk::SwapchainCreateFlagsKHR(), surface, imageCount, surfaceFormat.format, surfaceFormat.colorSpace,
		extent, 1, vk::ImageUsageFlagBits::eColorAttachment
	);

	if (famind.graphics.value() != famind.present.value())
	{
		swapCreateInfo.imageSharingMode = vk::SharingMode::eConcurrent;
		swapCreateInfo.queueFamilyIndexCount = uniqueFAMIND.size();
		swapCreateInfo.pQueueFamilyIndices = uniqueFAMIND.data();
	}
	else
	{
		swapCreateInfo.imageSharingMode = vk::SharingMode::eExclusive;
	}
	swapCreateInfo.preTransform = support.capabilities.currentTransform;
	swapCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	swapCreateInfo.presentMode = presentMode;
	swapCreateInfo.clipped = VK_TRUE;
	swapCreateInfo.oldSwapchain = vk::SwapchainKHR(nullptr);
}

void Engine::CreateSwapchain()
{
	if (debug) std::cout << YELLOW << "[Setup]" << RESET << "Creating the swapchain ";
	try {
		bundle.swapchain = logicalDevice.createSwapchainKHR(swapCreateInfo);
	}
	catch (vk::SystemError err) {
		std::cerr << RED << "[###]" << RESET << std::endl;
		error = true;
		finishSetup();
		std::abort();
	}
	std::cout << GREEN << "[###]" << RESET << std::endl;

	bundle.format = surfaceFormat.format;
	bundle.extent = extent;
}

void Engine::CreateImageViews()
{
	if (debug) std::cout << YELLOW << "[Setup]" << RESET << "Creating image views " << GREEN << "[###]" << RESET << std::endl;
	std::vector <vk::Image> images = logicalDevice.getSwapchainImagesKHR(bundle.swapchain);
	bundle.frames.resize(images.size());
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

		bundle.frames[i].image = images[i];
		bundle.frames[i].view = logicalDevice.createImageView(imageViewCreateInfo);
	}
	maxFramesInFlight = bundle.frames.size();
	frameNumber = 0;
}
#include "MaxwellsEngine.h"

MaxwellsEngine::~MaxwellsEngine()
{
	glfwDestroyWindow(window);
	glfwTerminate();
	
	logicalDevice.waitIdle();
	logicalDevice.destroyShaderModule(*vertexShader);
	logicalDevice.destroyShaderModule(*fragmentShader);

	logicalDevice.destroySemaphore(imageAvailable);
	logicalDevice.destroySemaphore(renderFinished);
	logicalDevice.destroyFence(inFlightFence);
	logicalDevice.destroyCommandPool(commandPool);

	logicalDevice.destroyPipeline(pipeline);
	logicalDevice.destroyPipelineLayout(layout);
	logicalDevice.destroyRenderPass(renderPass);

	for (const SwapChainFrame& frame : bundle.frames)
	{
		logicalDevice.destroyImageView(frame.view);
	}
	logicalDevice.destroySwapchainKHR(bundle.swapchain);
	logicalDevice.waitIdle();
	logicalDevice.destroy();
	instance.destroySurfaceKHR(surface);
	instance.destroy();

	//REMEMBER
	//undestroyed resources, unfinished setups, ETC.
	//cause nvoglv64.dll crashes
}

MaxwellsEngine::MaxwellsEngine()
{
	std::cout << YELLOW << "[Setup]" << RESET << "Constructor called, preparing the engine: " << std::endl;
}

MaxwellsEngine::MaxwellsEngine(bool _debug)
{
	debug = _debug;
	std::cout << YELLOW << "[Setup]" << RESET << "Constructor called, preparing the engine: " << std::endl;
}

void MaxwellsEngine::Setup()
{
	CreateWindow();
	CreateVulkanInstance();
	PickDevice();
	CreateSurface();
	CreateQueues();
	CreateLogicDevice();
	PreConfigSwapchain();
	CreateSwapchain();
	CreateImageViews();

	finishPrimarySetup();

	PipelineSetup();
	RenderSetup();

	finishSetup();
}

void MaxwellsEngine::WindowEventLoop()
{
	shouldClose = glfwWindowShouldClose(window);
	glfwPollEvents();
}

void MaxwellsEngine::Draw()
{
	logicalDevice.waitForFences(1, &inFlightFence, VK_TRUE, UINT64_MAX);
	logicalDevice.resetFences(1, &inFlightFence);

	uint32_t imageIndex{logicalDevice.acquireNextImageKHR(bundle.swapchain, UINT64_MAX, imageAvailable, nullptr).value};

	vk::CommandBuffer commandBuffer = bundle.frames[imageIndex].commandBuffer;

	commandBuffer.reset();

	recordDrawCommands(commandBuffer, imageIndex);

	vk::SubmitInfo submitinfo = {};
	vk::Semaphore waitSemaphores[] = {imageAvailable};
	vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
	submitinfo.waitSemaphoreCount = 1;
	submitinfo.pWaitSemaphores = waitSemaphores;
	submitinfo.pWaitDstStageMask = waitStages;
	submitinfo.commandBufferCount = 1;
	submitinfo.pCommandBuffers = &commandBuffer;
	vk::Semaphore signalSemaphores[] = {renderFinished};
	submitinfo.signalSemaphoreCount = 1;
	submitinfo.pSignalSemaphores = signalSemaphores;
	try {
		graphicsQueue.submit(submitinfo, inFlightFence);
	}
	catch (vk::SystemError err) {
		std::cerr << "Failed to draw command buffer" << std::endl;
		std::abort();
	}
	vk::PresentInfoKHR presentInfo = {};
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	vk::SwapchainKHR swapchains[] = {bundle.swapchain};
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapchains;
	presentInfo.pImageIndices = &imageIndex;

	presentQueue.presentKHR(presentInfo);
}

void MaxwellsEngine::recordDrawCommands(vk::CommandBuffer& commandBuffer, uint32_t imageIndex)
{
	vk::CommandBufferBeginInfo beginInfo = {};
	try {
		commandBuffer.begin(beginInfo);
	}
	catch (vk::SystemError err) {
		std::cerr << "Error whilst trying to start recording draw calls" << std::endl;
		std::abort();
	}

	vk::RenderPassBeginInfo passInfo = {};
	passInfo.renderPass = renderPass;
	passInfo.framebuffer = bundle.frames[imageIndex].frameBuffer;
	passInfo.renderArea.offset.x = 0;
	passInfo.renderArea.offset.y = 0;
	passInfo.renderArea.extent = extent;
	vk::ClearValue clearColor = { std::array<float, 4>{0, 0, 0, 1} };
	passInfo.clearValueCount = 1;
	passInfo.pClearValues = &clearColor;

	commandBuffer.beginRenderPass(passInfo, vk::SubpassContents::eInline);
	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
	commandBuffer.draw(3, 1, 0, 0);
	commandBuffer.endRenderPass();
	try {
		commandBuffer.end();
	}
	catch(vk::SystemError err) {
		std::cerr << "Error whilst ending the render pass" << std::endl;
		std::abort();
	}
}

void MaxwellsEngine::RenderSetup()
{
	//create frame buffer
	for (int i = 0; i < bundle.frames.size(); i++)
	{
		std::vector<vk::ImageView> attachments = {
			bundle.frames[i].view
		};
		vk::FramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.flags = vk::FramebufferCreateFlags();
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = attachments.size();
		framebufferInfo.width = extent.width;
		framebufferInfo.height = extent.height;
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.layers = 1;

		try {
			bundle.frames[i].frameBuffer = logicalDevice.createFramebuffer(framebufferInfo);
		}
		catch (vk::SystemError) {
			std::cerr << "Error whilst creating a framebuffer" << std::endl;
			std::abort();
		}
	}

	//create a command pool
	vk::CommandPoolCreateInfo commandPoolInfo = {};
	commandPoolInfo.flags = vk::CommandPoolCreateFlags() | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
	commandPoolInfo.queueFamilyIndex = famind.graphics.value();

	try {
		commandPool = logicalDevice.createCommandPool(commandPoolInfo);
	}
	catch (vk::SystemError err) {
		std::cerr << "Error whilst creating the command pool" << std::endl;
		std::abort();
	}

	//make command buffers
	vk::CommandBufferAllocateInfo commandBufferAllocInfo = {};
	commandBufferAllocInfo.commandPool = commandPool;
	commandBufferAllocInfo.level = vk::CommandBufferLevel::ePrimary;
	commandBufferAllocInfo.commandBufferCount = 1;

	for (int i = 0; i < bundle.frames.size(); i++)
	{
		try {
			bundle.frames[i].commandBuffer = logicalDevice.allocateCommandBuffers(commandBufferAllocInfo)[0];	
		}
		catch (vk::SystemError err) {
			std::cerr << "Error whilst allocating command buffers" << std::endl;
			std::abort();
		}
	}

	try {
		commandBuffer = logicalDevice.allocateCommandBuffers(commandBufferAllocInfo)[0];
	}
	catch (vk::SystemError err){
		std::cerr << "Error whilst allocating the main command buffer" << std::endl;
		std::abort();
	}

	//create semaphores
	vk::SemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.flags = vk::SemaphoreCreateFlags();

	try {
		imageAvailable = logicalDevice.createSemaphore(semaphoreCreateInfo);
	}
	catch (vk::SystemError err) {
		std::cerr << "Error whilst creating a semaphore" << std::endl;
		std::abort();
	}

	try {
		renderFinished = logicalDevice.createSemaphore(semaphoreCreateInfo);
	}
	catch (vk::SystemError err) {
		std::cerr << "Error whilst creating a semaphore" << std::endl;
		std::abort();
	}

	//create a fence
	vk::FenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.flags = vk::FenceCreateFlags() | vk::FenceCreateFlagBits::eSignaled;

	try {
		inFlightFence = logicalDevice.createFence(fenceCreateInfo);
	}
	catch (vk::SystemError err) {
		std::cerr << "Error whilst creating a fence" << std::endl;
		std::abort();
	}
}

void MaxwellsEngine::CreateShaderModule(std::string filepath, vk::Device device, vk::ShaderModule** ppShaderModule)
{
	std::cout << YELLOW << "[Setup]"<<MAGENTA<<"->"<<YELLOW<<"[Pipeline]"<<MAGENTA<<"->"<<YELLOW<<"[Create-Shader]" << RESET << "Creating shader module ";
	std::vector<char> sourceCode = SPIRV::LoadFile(filepath);
	vk::ShaderModuleCreateInfo createInfo = {};
	createInfo.flags = vk::ShaderModuleCreateFlags();
	createInfo.codeSize = sourceCode.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(sourceCode.data());

	vk::ShaderModule module;

	try {
		module = device.createShaderModule(createInfo);
	}
	catch (vk::SystemError err) {
		std::cerr << RED << "[###]" << RESET << std::endl;
		error = true;
		finishPipelineSetup();
		finishSetup();
		std::abort();
	}
	std::cout << GREEN << "[###]" << RESET << std::endl;

	*ppShaderModule = &module;
}

void MaxwellsEngine::PipelineSetup()
{
	vk::GraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.flags = vk::PipelineCreateFlags();
	std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;

	if (debug) std::cout << YELLOW << "[Setup]" << MAGENTA << "->" << YELLOW << "[Pipeline]" << RESET << "Creating a vertex input " << GREEN << "[###]" << RESET << std::endl;
	//Vertex Input
	vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.flags = vk::PipelineVertexInputStateCreateFlags();
	vertexInputInfo.vertexAttributeDescriptionCount = 0;
	vertexInputInfo.vertexBindingDescriptionCount = 0;
	pipelineInfo.pVertexInputState = &vertexInputInfo;

	if (debug) std::cout << YELLOW << "[Setup]" << MAGENTA << "->" << YELLOW << "[Pipeline]" << RESET << "Creating a input assembly " << GREEN << "[###]" << RESET << std::endl;
	//Input Assembly
	vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
	inputAssemblyInfo.flags = vk::PipelineInputAssemblyStateCreateFlags();
	inputAssemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;
	pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;

	//Vertex shader
	// in class declaration: vk::ShaderModule* vertexShader = NULL;
	CreateShaderModule("./Shaders/shaderV.spv", logicalDevice, &vertexShader);
	vk::PipelineShaderStageCreateInfo vertexShaderInfo = {};
	vertexShaderInfo.flags = vk::PipelineShaderStageCreateFlags();
	vertexShaderInfo.stage = vk::ShaderStageFlagBits::eVertex;
	vertexShaderInfo.module = *vertexShader;
	vertexShaderInfo.pName = "main";
	shaderStages.push_back(vertexShaderInfo);

	if (debug) std::cout << YELLOW << "[Setup]" << MAGENTA << "->" << YELLOW << "[Pipeline]" << RESET << "Creating the viewport " << GREEN << "[###]" << RESET << std::endl;
	//Viewport
	vk::Viewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = extent.width;
	viewport.height = extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vk::Rect2D scissor = {};
	scissor.offset.x = 0.0f;
	scissor.offset.y = 0.0f;
	scissor.extent = extent;
	vk::PipelineViewportStateCreateInfo viewportInfo = {};
	viewportInfo.flags = vk::PipelineViewportStateCreateFlags();
	viewportInfo.viewportCount = 1;
	viewportInfo.pViewports = &viewport;
	viewportInfo.scissorCount = 1;
	viewportInfo.pScissors = &scissor;
	pipelineInfo.pViewportState = &viewportInfo;

	if (debug) std::cout << YELLOW << "[Setup]" << MAGENTA << "->" << YELLOW << "[Pipeline]" << RESET << "Creating a rasterizer " << GREEN << "[###]" << RESET << std::endl;
	//Rasterization
	vk::PipelineRasterizationStateCreateInfo rasterizationInfo = {};
	rasterizationInfo.flags = vk::PipelineRasterizationStateCreateFlags();
	rasterizationInfo.depthClampEnable = VK_FALSE;
	rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizationInfo.polygonMode = vk::PolygonMode::eFill;
	rasterizationInfo.lineWidth = 1.0f;
	rasterizationInfo.cullMode = vk::CullModeFlagBits::eBack;
	rasterizationInfo.frontFace = vk::FrontFace::eClockwise;
	rasterizationInfo.depthBiasEnable = VK_FALSE;
	pipelineInfo.pRasterizationState = &rasterizationInfo;

	//Fragment shader
	//in class declaration: vk::ShaderModule* fragmentShader = NULL;
	CreateShaderModule("./Shaders/shaderF.spv", logicalDevice, &fragmentShader);
	vk::PipelineShaderStageCreateInfo fragmentShaderInfo = {};
	fragmentShaderInfo.flags = vk::PipelineShaderStageCreateFlags();
	fragmentShaderInfo.stage = vk::ShaderStageFlagBits::eFragment;
	fragmentShaderInfo.module = *fragmentShader;
	fragmentShaderInfo.pName = "main";
	shaderStages.push_back(fragmentShaderInfo);
	
	if (debug) std::cout << YELLOW << "[Setup]" << MAGENTA << "->" << YELLOW << "[Pipeline]" << RESET << "Setting shader stages " << GREEN << "[###]" << RESET << std::endl;
	//Set shaders
	pipelineInfo.stageCount = shaderStages.size();
	pipelineInfo.pStages = shaderStages.data();

	if (debug) std::cout << YELLOW << "[Setup]" << MAGENTA << "->" << YELLOW << "[Pipeline]" << RESET << "Creating a multisampler " << GREEN << "[###]" << RESET << std::endl;
	//Multisampling
	vk::PipelineMultisampleStateCreateInfo multisampleInfo = {};
	multisampleInfo.flags = vk::PipelineMultisampleStateCreateFlags();
	multisampleInfo.sampleShadingEnable = VK_FALSE;
	multisampleInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;
	pipelineInfo.pMultisampleState = &multisampleInfo;

	if (debug) std::cout << YELLOW << "[Setup]" << MAGENTA << "->" << YELLOW << "[Pipeline]" << RESET << "Specifying a color blender " << GREEN << "[###]" << RESET << std::endl;
	//Color blend
	vk::PipelineColorBlendAttachmentState attachment = {};
	attachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
	attachment.blendEnable = VK_FALSE;
	vk::PipelineColorBlendStateCreateInfo colorBlendingInfo = {};
	colorBlendingInfo.flags = vk::PipelineColorBlendStateCreateFlags();
	colorBlendingInfo.logicOpEnable = VK_FALSE;
	colorBlendingInfo.logicOp = vk::LogicOp::eCopy;
	colorBlendingInfo.attachmentCount = 1;
	colorBlendingInfo.pAttachments = &attachment;
	colorBlendingInfo.blendConstants[0] = 0.0f;
	colorBlendingInfo.blendConstants[1] = 0.0f;
	colorBlendingInfo.blendConstants[2] = 0.0f;
	colorBlendingInfo.blendConstants[3] = 0.0f;
	pipelineInfo.pColorBlendState = &colorBlendingInfo;



	if (debug) std::cout << YELLOW << "[Setup]" << MAGENTA << "->" << YELLOW << "[Pipeline]" << RESET << "Creating a pipeline layout ";
	//Create pipeline layout (move to a seperate function in future)
	//in class declaration: vk::PipelineLayout layout;
	vk::PipelineLayoutCreateInfo layoutInfo = {};
	layoutInfo.flags = vk::PipelineLayoutCreateFlags();
	layoutInfo.setLayoutCount = 0;
	layoutInfo.pushConstantRangeCount = 0;

	try {
		layout = logicalDevice.createPipelineLayout(layoutInfo);
	}
	catch (vk::SystemError) {
		std::cout << RED << "[###]" << RESET << std::endl;
		error = true;
		finishPipelineSetup();
		std::abort();
	}
	std::cout << GREEN << "[###]" << RESET << std::endl;

	if (debug) std::cout << YELLOW << "[Setup]" << MAGENTA << "->" << YELLOW << "[Pipeline]" << RESET << "Creating a renderpass ";
	//Create render pass (move to a seperate function in future)
	//in class declaration: vk::RenderPass renderPass;
	vk::AttachmentDescription colorAttachment = {};
	colorAttachment.flags = vk::AttachmentDescriptionFlags();
	colorAttachment.format = bundle.format;
	colorAttachment.samples = vk::SampleCountFlagBits::e1;
	colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
	colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
	colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
	colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
	colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
	colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

	vk::AttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

	vk::SubpassDescription subpass = {};
	subpass.flags = vk::SubpassDescriptionFlags();
	subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	vk::RenderPassCreateInfo renderpassInfo = {};
	renderpassInfo.flags = vk::RenderPassCreateFlags();
	renderpassInfo.attachmentCount = 1;
	renderpassInfo.pAttachments = &colorAttachment;
	renderpassInfo.subpassCount = 1;
	renderpassInfo.pSubpasses = &subpass;

	try {
		renderPass = logicalDevice.createRenderPass(renderpassInfo);
	}
	catch (vk::SystemError) {
		std::cerr << RED << "[###]" << RESET << std::endl;
		error = true;
		finishPipelineSetup();
		std::abort();
	}
	std::cout << GREEN << "[###]" << RESET << std::endl;

	//finish pipeline creation
	pipelineInfo.layout = layout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.basePipelineHandle = nullptr;
	
	try {
		pipeline = logicalDevice.createGraphicsPipeline(nullptr, pipelineInfo).value;
	}
	catch (vk::SystemError err) {
		error = true;
		finishPipelineSetup();
		std::abort();
	}
	finishPipelineSetup();
}

void MaxwellsEngine::CreateWindow()
{
	if (debug) std::cout << YELLOW << "[Setup]" << RESET << "Creating the window " << GREEN << "[###]" << RESET << std::endl;
	winData = { 0 };

	winData.width = 1280;
	winData.height = 720;
	winData.name = "Vulkan";

	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(winData.width, winData.height, winData.name.c_str(), NULL, NULL);
}

void MaxwellsEngine::CreateVulkanInstance()
{
	std::vector<const char*> validationLayers = {
		//"VK_LAYER_KHRONOS_validation",
	};

	if (debug) std::cout << YELLOW << "[Setup]" << RESET << "Creating a Vulkan instance ";
	uint32_t version{ 0 };
	version &= ~(0xFFFU);
	version = VK_MAKE_API_VERSION(0, 1, 0, 0);
	vkEnumerateInstanceVersion(&version);
	vk::ApplicationInfo appInfo = vk::ApplicationInfo(
		"VulkanEngine",
		version,
		"Doing it the hard way",
		version,
		version
	);
	uint32_t extensionCount;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&extensionCount);

	// Create a vector of const char* from glfwExtensions
	std::vector<const char*> extensions(glfwExtensions, glfwExtensions + extensionCount);

	vk::InstanceCreateInfo createInfo = vk::InstanceCreateInfo(
		vk::InstanceCreateFlags(),
		&appInfo,
		implicit_cast<uint32_t>(validationLayers.size()), validationLayers.data(),
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

void MaxwellsEngine::PickDevice()
{
	bool foundDevice = false;
	if (debug) std::cout << YELLOW << "[Setup]" << RESET << "Picking the device:" << std::endl;
	std::vector<vk::PhysicalDevice> availableDevices = instance.enumeratePhysicalDevices();
	for (const vk::PhysicalDevice& devices : availableDevices)
	{
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

void MaxwellsEngine::CreateSurface()
{
	if (debug) std::cout << YELLOW << "[Setup]" << RESET << "Creating the surface " << GREEN << "[###]" << RESET << std::endl;
	VkSurfaceKHR c_surface;
	glfwCreateWindowSurface(instance, window, NULL, &c_surface);
	surface = c_surface;
}

void MaxwellsEngine::CreateQueues()
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

void MaxwellsEngine::CreateLogicDevice()
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

void MaxwellsEngine::PreConfigSwapchain()
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

	extent.setWidth(implicit_cast<uint32_t>(winData.width));
	extent.setHeight(implicit_cast<uint32_t>(winData.height));
	bool set3 = false;
	if (support.capabilities.currentExtent.width != UINT32_MAX) {
		extent = support.capabilities.currentExtent;
		set3 = true;
	}
	else {
		extent.width = std::min(
			(int)support.capabilities.maxImageExtent.width,
			std::max((int)support.capabilities.minImageExtent.width, winData.width)
		);
		extent.height = std::min(
			(int)support.capabilities.maxImageExtent.height,
			std::max((int)support.capabilities.minImageExtent.height, winData.height)
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

void MaxwellsEngine::CreateSwapchain()
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

void MaxwellsEngine::CreateImageViews()
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
}
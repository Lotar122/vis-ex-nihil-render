#include "Engine.hpp"
#include "VertexBuffer/VertexBuffer.hpp"

#ifdef _DEBUG
#define DEBUGFLAG true
#else
#define DEBUGFLAG false
#endif

using namespace nihil;

Engine::Engine(bool _debug)
{
	debug = _debug;

	std::cout << YELLOW << "[Setup]" << RESET << "Constructor called, preparing the engine: " << std::endl;

	get = new Proxy(
		&instance,
		&device,
		&logicalDevice,
		&renderPass,
		&pipeline,
		&commandPool,
		&commandBuffer,
		&swapchainBundle
	);
}
Engine::Engine()
{
	debug = false;

	std::cout << YELLOW << "[Setup]" << RESET << "Constructor called, preparing the engine: " << std::endl;

	get = new Proxy(
		&instance,
		&device,
		&logicalDevice,
		&renderPass,
		&pipeline,
		&commandPool,
		&commandBuffer,
		&swapchainBundle
	);
}
Engine::~Engine()
{
	logicalDevice.waitIdle();
	logicalDevice.destroyShaderModule(*vertexShader);
	logicalDevice.destroyShaderModule(*fragmentShader);
	logicalDevice.destroyCommandPool(commandPool);

	logicalDevice.destroyPipeline(pipeline);
	logicalDevice.destroyPipelineLayout(layout);
	logicalDevice.destroyRenderPass(renderPass);

	destroySwapchain();

	delete vertexBuffer;

	logicalDevice.waitIdle();
	logicalDevice.destroy();
	instance.destroySurfaceKHR(surface);
	instance.destroy();

	//REMEMBER
	//undestroyed resources, unfinished setups, ETC.
	//cause nvoglv64.dll crashes
}

void Engine::destroySwapchain()
{
	std::cout << std::endl << YELLOW << "[Setup]" << RESET << " Destroying the swapchain " << GREEN << "[###]" << std::endl;
	logicalDevice.waitIdle();
	for (const SwapChainFrame& frame : swapchainBundle.frames)
	{
		logicalDevice.destroySemaphore(frame.imageAvailable);
		logicalDevice.destroySemaphore(frame.renderFinished);
		logicalDevice.destroyFence(frame.inFlightFence);
		logicalDevice.destroyImageView(frame.view);
		logicalDevice.destroyFramebuffer(frame.frameBuffer);
	}
	logicalDevice.destroySwapchainKHR(swapchainBundle.swapchain);
}

void Engine::RecreateSwapchain()
{
	int* pWidth = new int(0);
	int* pHeight = new int(0);
	glfwGetFramebufferSize(const_cast<GLFWwindow*>(app->get->window), pWidth, pHeight);
	app->SetWidth(*pWidth);
	app->SetHeight(*pHeight);
	while (app->get->width == 0 || app->get->height == 0)
	{
		glfwGetFramebufferSize(const_cast<GLFWwindow*>(app->get->window), pWidth, pHeight);
		app->SetWidth(*pWidth);
		app->SetHeight(*pHeight);
		glfwWaitEvents();
	}
	delete pWidth, pHeight;
	logicalDevice.waitIdle();
	destroySwapchain();
	SwapchainConfigCreateInfo configInfo = {};
	configInfo.preferredBuffering = BufferingMode::eTriple;
	configInfo.windowHeight = *app->get->height;
	configInfo.windowWidth = *app->get->width;
	swapchainConfiguration = CreateSwapchainConfiguration(configInfo);
	CreateSwapchain(swapchainConfiguration);
	CreateImageViews();
	createFrameBuffers();
	createSyncObjects();
	createFrameCommandBuffers();
}

void Engine::Setup()
{
	if (app == NULL) { std::cerr << "App is nullptr" << std::endl; std::abort(); }
	SetupDeafult();
}

void Engine::SetupDeafult()
{
	if (app == NULL) { std::cerr << "App is nullptr" << std::endl; std::abort(); }

	VulkanInstanceCreateInfo instanceInfo = {};
	instanceInfo.appName = *app->get->name;
	instanceInfo.appVersion = *app->get->appVersion;
	instanceInfo.vulkanVersion = *app->get->vulkanVersion;
	//deafult false
	instanceInfo.validationLayers = true;

	//Creation of the vulkan instance
	CreateVulkanInstance(instanceInfo);

	CreateVulkanSurface(app->get->window);

	PickPhysicalDevice();

	CreateVulkanQueues();

	CreateVulkanLogicalDevice();

	nihil::SwapchainConfigCreateInfo swapchainConfigCreateInfo = {};
	swapchainConfigCreateInfo.preferredBuffering = BufferingMode::eTriple;
	swapchainConfigCreateInfo.windowWidth = *app->get->width;
	swapchainConfigCreateInfo.windowHeight = *app->get->height;

	nihil::SwapchainConfiguration swapchainConfig = CreateSwapchainConfiguration(swapchainConfigCreateInfo);

	CreateSwapchain(swapchainConfig);

	CreateImageViews();

	finishPrimarySetup();

	//customize shaders in future with the component system
	PipelineSetup();

	//make the Draw function interactable
	RenderSetup();

	finishSetup();

	vertexBuffer = new VertexBuffer(this, &app->screenRatio);
}

void Engine::setApp(App* _app)
{
	app = _app;
}
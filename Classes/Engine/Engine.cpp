#include "Engine.hpp"
#include "VertexBuffer/VertexBuffer.hpp"

using namespace nihil;

#ifdef _DEBUG
#define DEBUGFLAG true
#else
#define DEBUGFLAG false
#endif

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
		&bundle
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
		&bundle
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
	for (const SwapChainFrame& frame : bundle.frames)
	{
		logicalDevice.destroySemaphore(frame.imageAvailable);
		logicalDevice.destroySemaphore(frame.renderFinished);
		logicalDevice.destroyFence(frame.inFlightFence);
		logicalDevice.destroyImageView(frame.view);
		logicalDevice.destroyFramebuffer(frame.frameBuffer);
	}
	logicalDevice.destroySwapchainKHR(bundle.swapchain);
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
	PreConfigSwapchain();
	CreateSwapchain();
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

	InstanceCreateInfo instanceCreateInfo = {};
	instanceCreateInfo.AppName = "Sample App";
	Version appVersion = {};
	appVersion.make_version(0, 1, 0, 0);
	instanceCreateInfo.AppVersion = appVersion;
	Version vulkanVersion = {};
	vulkanVersion.make_version(0, 1, 0, 0);
	instanceCreateInfo.VulkanVersion = vulkanVersion;
	std::string validationlayers = "VK_LAYER_KHRONOS_validation";
	if (!DEBUGFLAG) validationlayers = "";
	std::vector<const char*> validationLayers = {
		validationlayers.c_str()
	};
	instanceCreateInfo.validationLayers = validationLayers;
	CreateVulkanInstance(instanceCreateInfo);
	PickDevice();
	CreateSurface();

	//implement in future as for now no functionality needs to customize this
	CreateQueues();

	//same as queues
	CreateLogicDevice();

	//enable the user to select the options as triple buffering double buffering ETC.
	PreConfigSwapchain();

	//implement in future as for now no functionality needs to customize this
	CreateSwapchain();
	CreateImageViews();

	finishPrimarySetup();

	//customize shaders in future with the component system
	PipelineSetup();

	//make the Draw function interactable
	RenderSetup();

	finishSetup();

	vertexBuffer = new VertexBuffer(this);
}

void Engine::setApp(App* _app)
{
	app = _app;
}
#include "Renderer.hpp"

#include "Classes/Engine/Engine.hpp"
#include "Classes/Engine/Engine.Structs.InData.hpp"

#include "Classes/Buffer/Buffer.hpp"

namespace nihil::graphics {

	Renderer::Renderer(Engine* _engine) 
	{
		engine = _engine;

		famind = engine->famind;
		graphicsQueue = engine->graphicsQueue;
		presentQueue = engine->presentQueue;
		uniqueFAMIND = engine->uniqueFAMIND;

		SwapchainConfigCreateInfo swapchainConfigCreateInfo = {};
		swapchainConfigCreateInfo.preferredBuffering = BufferingMode::eTriple;
		swapchainConfigCreateInfo.windowWidth = *engine->app->get->width;
		swapchainConfigCreateInfo.windowHeight = *engine->app->get->height;

		SwapchainConfiguration swapchainConfig = CreateSwapchainConfiguration(swapchainConfigCreateInfo);

		CreateSwapchain(swapchainConfig);

		CreateImageViews();

		swapchainBundle.depthFormat = vk::Format::eD32Sfloat;
		createDepthBuffers();

		engine->finishPrimarySetup();

		//customize shaders in future with the component system
		PipelineSetup();

		createCommandPools();
		createFrameCommandBuffers();
		createMainCommandBuffer();
		createDepthBuffers();
		createFrameBuffers();
		createSyncObjects();

		//loadingThread.join();

		engine->finishSetup();
	}
	Renderer::~Renderer() 
	{
		engine->logicalDevice.waitIdle();
		engine->logicalDevice.destroyShaderModule(*vertexShader);
		engine->logicalDevice.destroyShaderModule(*fragmentShader);
		engine->logicalDevice.destroyCommandPool(commandPool);

		engine->logicalDevice.destroyPipeline(pipeline);
		engine->logicalDevice.destroyPipelineLayout(layout);
		engine->logicalDevice.destroyRenderPass(renderPass);

		destroySwapchain();

		engine->logicalDevice.waitIdle();
		engine->logicalDevice.destroy();
		engine->instance.destroySurfaceKHR(engine->surface);
		engine->instance.destroy();

		//REMEMBER
		//undestroyed resources, unfinished setups, ETC.
		//cause nvoglv64.dll crashes
	}

	void Renderer::destroySwapchain()
	{
		std::cout << std::endl << YELLOW << "[Setup]" << RESET << " Destroying the swapchain " << GREEN << "[###]" << std::endl;
		engine->logicalDevice.waitIdle();
		for (const SwapChainFrame& frame : swapchainBundle.frames)
		{
			engine->logicalDevice.destroySemaphore(frame.imageAvailable);
			engine->logicalDevice.destroySemaphore(frame.renderFinished);
			engine->logicalDevice.destroyFence(frame.inFlightFence);
			engine->logicalDevice.destroyImageView(frame.view);
			engine->logicalDevice.destroyFramebuffer(frame.frameBuffer);
			engine->logicalDevice.destroyImage(frame.depthBuffer);
			engine->logicalDevice.destroyImageView(frame.depthBufferView);
			engine->logicalDevice.freeMemory(frame.depthBufferMemory);
		}
		engine->logicalDevice.destroySwapchainKHR(swapchainBundle.swapchain);
	}
}
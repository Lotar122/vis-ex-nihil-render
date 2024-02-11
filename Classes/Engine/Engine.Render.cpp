#include "Engine.hpp"

#include <chrono>
#include <thread>

#include "VertexBuffer/VertexBuffer.hpp"

using namespace nihil;

double lastTime = 0.0;
uint64_t frameCount = 0;

uint16_t calculateFPS(GLFWwindow* window, Engine* engine) {
	double currentTime = glfwGetTime();
	double delta = currentTime - lastTime;
	frameCount++;

	if (delta >= 1.0) {
		double fps = static_cast<double>(frameCount) / delta;
		frameCount = 0;
		lastTime = currentTime;

		// Update window title with FPS value
		std::string title = "FPS: " + std::to_string(fps);
		glfwSetWindowTitle(window, title.c_str());

		return fps;
	}
	return (uint16_t)engine->FPS;
}

void Engine::Draw(Scene* scene)
{
	logicalDevice.waitForFences(1, &swapchainBundle.frames[frameNumber].inFlightFence, VK_TRUE, UINT64_MAX);
	uint32_t imageIndex{ 0 };

	//vertexBuffer->Data[1] += nstd::USC::NDC_u(0.1, app->screenRatio, nstd::WidthHeightEnum::Height);

	//vertexBuffer->refresh(vertexBuffer->Data);

	try {
		vk::ResultValue acquire = logicalDevice.acquireNextImageKHR(swapchainBundle.swapchain, UINT64_MAX, swapchainBundle.frames[frameNumber].imageAvailable, nullptr);
		imageIndex = acquire.value;
	}
	catch (vk::OutOfDateKHRError err) {
		RecreateSwapchain();
		return;
	}

	vk::CommandBuffer commandBuffer = swapchainBundle.frames[imageIndex].commandBuffer;

	commandBuffer.reset();

	recordDrawCommands(commandBuffer, imageIndex, scene);

	vk::SubmitInfo submitinfo = {};
	vk::Semaphore waitSemaphores[] = { swapchainBundle.frames[frameNumber].imageAvailable };
	vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
	submitinfo.waitSemaphoreCount = 1;
	submitinfo.pWaitSemaphores = waitSemaphores;
	submitinfo.pWaitDstStageMask = waitStages;
	submitinfo.commandBufferCount = 1;
	submitinfo.pCommandBuffers = &swapchainBundle.frames[frameNumber].commandBuffer;
	vk::Semaphore signalSemaphores[] = { swapchainBundle.frames[frameNumber].renderFinished };
	submitinfo.signalSemaphoreCount = 1;
	submitinfo.pSignalSemaphores = signalSemaphores;

	logicalDevice.resetFences(1, &swapchainBundle.frames[frameNumber].inFlightFence);
	try {
		graphicsQueue.submit(submitinfo, swapchainBundle.frames[frameNumber].inFlightFence);
	}
	catch (vk::SystemError err) {
		std::cerr << "Failed to draw command buffer" << std::endl;
		std::abort();
	}
	vk::PresentInfoKHR presentInfo = {};
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	vk::SwapchainKHR swapchains[] = { swapchainBundle.swapchain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapchains;
	presentInfo.pImageIndices = &imageIndex;

	vk::Result presentResult;
	try {
		presentResult = presentQueue.presentKHR(presentInfo);
	}
	catch (vk::OutOfDateKHRError err) {
		presentResult = vk::Result::eErrorOutOfDateKHR;
	}
	if (presentResult == vk::Result::eErrorOutOfDateKHR || presentResult == vk::Result::eSuboptimalKHR)
	{
		RecreateSwapchain();
		return;
	}

	frameNumber = (frameNumber + 1) % maxFramesInFlight;

	//uint16_t currentFPS = calculateFPS(const_cast<GLFWwindow*>(app->get->window), this);
}

void Engine::recordDrawCommands(vk::CommandBuffer& commandBuffer, uint32_t imageIndex, Scene* scene)
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
	passInfo.framebuffer = swapchainBundle.frames[imageIndex].frameBuffer;
	passInfo.renderArea.offset.x = 0;
	passInfo.renderArea.offset.y = 0;
	passInfo.renderArea.extent = swapchainBundle.extent;
	vk::ClearValue clearColor = { std::array<float, 4>{0, 0, 0, 1} };
	passInfo.clearValueCount = 1;
	passInfo.pClearValues = &clearColor;

	commandBuffer.beginRenderPass(passInfo, vk::SubpassContents::eInline);
	commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
	vk::Buffer vertexBuffers[] = {
		vertexBuffer->buffer.buffer
	};
	vk::DeviceSize offsets[] = {
		0
	};
	commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
	for (glm::vec3 position : scene->triangleVertices)
	{
		glm::mat4 model = glm::translate(glm::mat4(1.0f), position);
		ObjectData data = {};
		data.model = model;
		srand((unsigned) std::time(NULL));
		data.time = std::rand();
		commandBuffer.pushConstants(layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(ObjectData), &data);
		commandBuffer.draw(vertexBuffer->Data.size(), 1, 0, 0);
	}
	commandBuffer.endRenderPass();
	try {
		commandBuffer.end();
	}
	catch (vk::SystemError err) {
		std::cerr << "Error whilst ending the render pass" << std::endl;
		std::abort();
	}
}

void Engine::RenderSetup()
{
	createCommandPools();
	createFrameCommandBuffers();
	createMainCommandBuffer();
	createFrameBuffers();
	createSyncObjects();
}

void Engine::createCommandPools()
{
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
}

void Engine::createSyncObjects()
{
	//create semaphores
	vk::SemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.flags = vk::SemaphoreCreateFlags();

	//do it for each frame
	for (SwapChainFrame& frame : swapchainBundle.frames)
	{
		try {
			frame.imageAvailable = logicalDevice.createSemaphore(semaphoreCreateInfo);
		}
		catch (vk::SystemError err) {
			std::cerr << "Error whilst creating a semaphore" << std::endl;
			std::abort();
		}

		try {
			frame.renderFinished = logicalDevice.createSemaphore(semaphoreCreateInfo);
		}
		catch (vk::SystemError err) {
			std::cerr << "Error whilst creating a semaphore" << std::endl;
			std::abort();
		}

		//create a fence
		vk::FenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.flags = vk::FenceCreateFlags() | vk::FenceCreateFlagBits::eSignaled;

		try {
			frame.inFlightFence = logicalDevice.createFence(fenceCreateInfo);
		}
		catch (vk::SystemError err) {
			std::cerr << "Error whilst creating a fence" << std::endl;
			std::abort();
		}
	}
}

void Engine::createFrameCommandBuffers()
{
	//make command buffers
	vk::CommandBufferAllocateInfo commandBufferAllocInfo = {};
	commandBufferAllocInfo.commandPool = commandPool;
	commandBufferAllocInfo.level = vk::CommandBufferLevel::ePrimary;
	commandBufferAllocInfo.commandBufferCount = 1;

	for (int i = 0; i < swapchainBundle.frames.size(); i++)
	{
		try {
			swapchainBundle.frames[i].commandBuffer = logicalDevice.allocateCommandBuffers(commandBufferAllocInfo)[0];
		}
		catch (vk::SystemError err) {
			std::cerr << "Error whilst allocating command buffers" << std::endl;
			std::abort();
		}
	}
}

void Engine::createMainCommandBuffer()
{
	//make command buffers
	vk::CommandBufferAllocateInfo commandBufferAllocInfo = {};
	commandBufferAllocInfo.commandPool = commandPool;
	commandBufferAllocInfo.level = vk::CommandBufferLevel::ePrimary;
	commandBufferAllocInfo.commandBufferCount = 1;

	try {
		commandBuffer = logicalDevice.allocateCommandBuffers(commandBufferAllocInfo)[0];
	}
	catch (vk::SystemError err) {
		std::cerr << "Error whilst allocating the main command buffer" << std::endl;
		std::abort();
	}
}

void Engine::createFrameBuffers()
{
	//create frame buffer
	for (int i = 0; i < swapchainBundle.frames.size(); i++)
	{
		std::vector<vk::ImageView> attachments = {
			swapchainBundle.frames[i].view
		};
		vk::FramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.flags = vk::FramebufferCreateFlags();
		framebufferInfo.renderPass = renderPass;
		framebufferInfo.attachmentCount = attachments.size();
		framebufferInfo.width = swapchainBundle.extent.width;
		framebufferInfo.height = swapchainBundle.extent.height;
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.layers = 1;

		try {
			swapchainBundle.frames[i].frameBuffer = logicalDevice.createFramebuffer(framebufferInfo);
		}
		catch (vk::SystemError) {
			std::cerr << "Error whilst creating a framebuffer" << std::endl;
			std::abort();
		}
	}
}


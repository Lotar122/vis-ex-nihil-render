#include "Engine.h"

void Engine::Draw()
{
	logicalDevice.waitForFences(1, &bundle.frames[frameNumber].inFlightFence, VK_TRUE, UINT64_MAX);
	logicalDevice.resetFences(1, &bundle.frames[frameNumber].inFlightFence);

	uint32_t imageIndex{ logicalDevice.acquireNextImageKHR(bundle.swapchain, UINT64_MAX, bundle.frames[frameNumber].imageAvailable, nullptr).value };

	vk::CommandBuffer commandBuffer = bundle.frames[imageIndex].commandBuffer;

	commandBuffer.reset();

	recordDrawCommands(commandBuffer, imageIndex);

	vk::SubmitInfo submitinfo = {};
	vk::Semaphore waitSemaphores[] = { bundle.frames[frameNumber].imageAvailable };
	vk::PipelineStageFlags waitStages[] = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
	submitinfo.waitSemaphoreCount = 1;
	submitinfo.pWaitSemaphores = waitSemaphores;
	submitinfo.pWaitDstStageMask = waitStages;
	submitinfo.commandBufferCount = 1;
	submitinfo.pCommandBuffers = &bundle.frames[frameNumber].commandBuffer;
	vk::Semaphore signalSemaphores[] = { bundle.frames[frameNumber].renderFinished };
	submitinfo.signalSemaphoreCount = 1;
	submitinfo.pSignalSemaphores = signalSemaphores;
	try {
		graphicsQueue.submit(submitinfo, bundle.frames[frameNumber].inFlightFence);
	}
	catch (vk::SystemError err) {
		std::cerr << "Failed to draw command buffer" << std::endl;
		std::abort();
	}
	vk::PresentInfoKHR presentInfo = {};
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	vk::SwapchainKHR swapchains[] = { bundle.swapchain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapchains;
	presentInfo.pImageIndices = &imageIndex;

	presentQueue.presentKHR(presentInfo);

	frameNumber = (frameNumber + 1) % maxFramesInFlight;
}

void Engine::recordDrawCommands(vk::CommandBuffer& commandBuffer, uint32_t imageIndex)
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
	catch (vk::SystemError err) {
		std::cerr << "Error whilst ending the render pass" << std::endl;
		std::abort();
	}
}

void Engine::RenderSetup()
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
	catch (vk::SystemError err) {
		std::cerr << "Error whilst allocating the main command buffer" << std::endl;
		std::abort();
	}

	//create semaphores
	vk::SemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.flags = vk::SemaphoreCreateFlags();

	//do it for each frame
	for (SwapChainFrame& frame : bundle.frames)
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
#include "Renderer.hpp"
#include "Classes/Engine/Engine.hpp"
#include "Classes/Buffer/Buffer.hpp"

namespace nihil::graphics {
	void Renderer::Draw()
	{
		engine->logicalDevice.waitForFences(1, &swapchainBundle.frames[frameNumber].inFlightFence, VK_TRUE, UINT64_MAX);
		uint32_t imageIndex{ 0 };

		//vertexBuffer->Data[1] += nstd::USC::NDC_u(0.1, app->screenRatio, nstd::WidthHeightEnum::Height);

		//vertexBuffer->refresh(vertexBuffer->Data);

		try {
			vk::ResultValue acquire = engine->logicalDevice.acquireNextImageKHR(swapchainBundle.swapchain, UINT64_MAX, swapchainBundle.frames[frameNumber].imageAvailable, nullptr);
			imageIndex = acquire.value;
		}
		catch (vk::OutOfDateKHRError err) {
			RecreateSwapchain();
			return;
		}

		vk::CommandBuffer commandBuffer = swapchainBundle.frames[imageIndex].commandBuffer;

		commandBuffer.reset();

		recordDrawCommands(commandBuffer, imageIndex);

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

		engine->logicalDevice.resetFences(1, &swapchainBundle.frames[frameNumber].inFlightFence);
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
	uint64_t count;
	void Renderer::recordDrawCommands(vk::CommandBuffer& commandBuffer, uint32_t imageIndex)
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
		vk::ClearValue clearValues[2];
		clearValues[0].color = { 0.0f,0.0f,0.0f,1.0f };
		clearValues[1].depthStencil = vk::ClearDepthStencilValue(1.0f, 0.0f);
		passInfo.clearValueCount = 2;
		passInfo.pClearValues = clearValues;

		commandBuffer.beginRenderPass(passInfo, vk::SubpassContents::eInline);
		commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
		vk::Buffer vertexBuffers[] = {
			vertexBuffer->buffer.buffer
		};
		vk::DeviceSize offsets[] = {
			0
		};

		commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
		commandBuffer.bindIndexBuffer(indexBuffer->buffer.buffer, 0, vk::IndexType::eUint32);

		count++;

		ObjectData data = {};

		data.proj = glm::perspectiveRH(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);
		data.trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, -10.0f));
		data.trans = glm::rotate(data.trans, glm::radians(0.1f * count), glm::vec3(0.0f, 1.0f, 0.0f));
		data.trans = glm::rotate(data.trans, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		data.trans = glm::scale(data.trans, glm::vec3(0.05f, 0.05f, 0.05f));

		data.pre = data.proj * data.trans;

		commandBuffer.pushConstants(layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(ObjectData), &data);

		if (*engine->app->get->width != viewport.width || *engine->app->get->height != viewport.height)
		{
			viewport = vk::Viewport(
				0.0f, 0.0f,
				static_cast<float>(*engine->app->get->width), static_cast<float>(*engine->app->get->height),
				0.0f, 1.0f
			);

			scissor = vk::Rect2D(vk::Offset2D(0, 0), vk::Extent2D(*engine->app->get->width, *engine->app->get->height));
		}

		commandBuffer.setViewport(0, 1, &viewport);
		commandBuffer.setScissor(0, 1, &scissor);

		commandBuffer.drawIndexed(static_cast<uint32_t>(indexBuffer->Data.size()), 1, 0, 0, 0);

		commandBuffer.endRenderPass();
		try {
			commandBuffer.end();
		}
		catch (vk::SystemError err) {
			std::cerr << "Error whilst ending the render pass" << std::endl;
			std::abort();
		}
	}
}
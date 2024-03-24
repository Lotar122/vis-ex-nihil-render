#include "Renderer.hpp"
#include "Classes/Engine/Engine.hpp"
#include "Classes/Buffer/Buffer.hpp"
#include "Classes/Model/Model.hpp"

namespace nihil::graphics {
	void Renderer::Draw(std::vector<nstd::Component>& modelArr)
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

		recordDrawCommands(commandBuffer, imageIndex, modelArr);

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
	void Renderer::recordDrawCommands(vk::CommandBuffer& commandBuffer, uint32_t imageIndex, std::vector<nstd::Component>& modelArr)
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

		//perform drawing for all models (change to the rendering objects later)
		for (const nstd::Component& comp : modelArr)
		{
			Model* model = (Model*)comp.data.any;
			drawBuffer(model->vBuffer, model->iBuffer, commandBuffer, model);
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

	void Renderer::drawBuffer(
		Buffer<float, vk::BufferUsageFlagBits::eVertexBuffer>* _vertexBuffer, 
		Buffer<uint32_t, vk::BufferUsageFlagBits::eIndexBuffer>* _indexBuffer, 
		vk::CommandBuffer& _commandBuffer, 
		Model* model
	)
	{
		vk::Buffer vertexBuffers[] = {
			_vertexBuffer->buffer.buffer
		};
		vk::DeviceSize offsets[] = {
			0
		};

		_commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
		_commandBuffer.bindIndexBuffer(_indexBuffer->buffer.buffer, 0, vk::IndexType::eUint32);

		count++;

		ObjectData data = {};

		data.proj = glm::perspectiveRH(glm::radians(90.0f), 1.0f, 0.1f, 100.0f);
		data.trans = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f));
		data.trans = glm::rotate(data.trans, glm::radians(0.1f * count), glm::vec3(0.0f, 1.0f, 0.0f));
		data.trans = glm::rotate(data.trans, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));

		//remove later, only to make thing smaller
		data.trans = glm::scale(data.trans, glm::vec3(0.05f, 0.05f, 0.05f));

		if (model != NULL)
		{
			data.pre = data.proj * data.trans * model->deafultTransform;
		}
		else
		{
			data.pre = data.proj * data.trans;
		}

		_commandBuffer.pushConstants(layout, vk::ShaderStageFlagBits::eVertex, 0, sizeof(ObjectData), &data);

		_commandBuffer.drawIndexed(static_cast<uint32_t>(_indexBuffer->Data.size()), 1, 0, 0, 0);
	}
}
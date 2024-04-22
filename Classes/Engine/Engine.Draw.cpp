#include "Engine.hpp"

namespace nihil::graphics
{
	void Engine::queueInstancedDraw(
		Model* model,
		Buffer<float, vk::BufferUsageFlagBits::eVertexBuffer>* instanceBuffer,
		uint32_t pipeline
	)
	{
		InstancedDrawData* data = new InstancedDrawData();
		data->model = model;
		data->instanceBuffer = instanceBuffer;

		Command drawCommand = {};
		drawCommand.commandType = CommandType::DrawInstanced;
		drawCommand.data = (void*)data;
		drawCommand.targetPipeline = pipeline;

		commandQueue.push_back(drawCommand);
	}
}
#pragma once
#include <iostream>
#include "Classes/Model/Model.hpp"

namespace nihil::graphics 
{
	enum class CommandType {
		DrawBuffer,
		DrawInstanced,
		DrawModel
	};
	struct Command {
		CommandType commandType;
		void* data;
		uint32_t targetPipeline;
	};
	
	struct InstancedDrawData {
		Model* model;
		Buffer<float, vk::BufferUsageFlagBits::eVertexBuffer>* instanceBuffer;
	};
}
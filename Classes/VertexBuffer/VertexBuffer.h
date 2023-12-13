#pragma once
#include "Engine/Engine.h"
#include "Memory/Memory.h"
#include <vulkan/vulkan.hpp>
class VertexBuffer
{
public:
	Engine* engine;
	std::vector<float> Data;
	Buffer buffer;
	VertexBuffer(Engine* _engine)
	{
		engine = _engine;

		Data = {
			0.0f, -0.05f, 0.0f, 0.0f, 0.0f, 1.0f,
			0.05f, 0.05f, 0.0f, 0.0f, 0.0f, 1.0f,
			-0.05f, 0.05f, 0.0f, 0.0f, 0.0f, 1.0f
		};

		buffer = Memory::CreateBuffer(sizeof(float) * Data.size(), vk::BufferUsageFlagBits::eVertexBuffer, engine);
		void* memoryLocation = engine->GetLogicalDevice()->mapMemory(buffer.memory, 0, sizeof(float) * Data.size());
		memcpy(memoryLocation, Data.data(), sizeof(float) * Data.size());
		engine->GetLogicalDevice()->unmapMemory(buffer.memory);
	}
	~VertexBuffer()
	{
		engine->GetLogicalDevice()->destroyBuffer(buffer.buffer);
		engine->GetLogicalDevice()->freeMemory(buffer.memory);
	}
};
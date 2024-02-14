#pragma once
#include "Classes/Engine/Engine.hpp"
#include "Classes/Memory/Memory.hpp"
#include <vulkan/vulkan.hpp>

#include <vector>

#include "nihil-standard/nstd.hpp"

namespace nihil {
	class VertexBuffer
	{
	public:
		using USC = nstd::USC;
		using XYZ = nstd::WidthHeightEnum;
		Engine* engine;
		std::vector<float> Data;
		Buffer buffer;
		uint32_t bufferSize;
		nstd::ScreenRatio* screenRatio;
		VertexBuffer(Engine* _engine, nstd::ScreenRatio* ratio)
		{
			screenRatio = ratio;
			engine = _engine;

			nstd::OBJ objobj;
			std::string path = "./cube.obj";
			objobj.Load(path, engine->app->screenRatio);
			Data = objobj.verticesRender;

			buffer = Memory::CreateBuffer(sizeof(float) * Data.size(), vk::BufferUsageFlagBits::eVertexBuffer, engine);
			void* memoryLocation = engine->get->logicalDevice->mapMemory(buffer.memory, 0, sizeof(float) * Data.size());
			memcpy(memoryLocation, Data.data(), sizeof(float) * Data.size());
			engine->get->logicalDevice->unmapMemory(buffer.memory);
		}
		~VertexBuffer()
		{
			engine->get->logicalDevice->destroyBuffer(buffer.buffer);
			engine->get->logicalDevice->freeMemory(buffer.memory);
		}
		void refresh(std::vector<float> _Data)
		{
			bufferSize = static_cast<uint32_t>(_Data.size() * sizeof(float));
			Data = _Data;
			buffer = Memory::CreateBuffer(bufferSize, vk::BufferUsageFlagBits::eVertexBuffer, engine);

			// Map buffer memory
			void* data;
			engine->get->logicalDevice->mapMemory(buffer.memory, 0, bufferSize, {}, &data);

			// Copy data to the mapped memory
			memcpy(data, _Data.data(), static_cast<size_t>(bufferSize));

			// Unmap buffer memory
			engine->get->logicalDevice->unmapMemory(buffer.memory);
		}
	};
}
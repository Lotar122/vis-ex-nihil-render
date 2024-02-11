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
		Engine* engine;
		std::vector<float> Data;
		Buffer buffer;
		nstd::ScreenRatio* screenRatio;
		VertexBuffer(Engine* _engine, nstd::ScreenRatio* ratio)
		{
			screenRatio = ratio;
			engine = _engine;

			Data = {
				//POS                                                   COLOR
				nstd::USC::NDC_u(-500, *screenRatio, nstd::WidthHeightEnum::Width), nstd::USC::NDC_u(-500, *screenRatio, nstd::WidthHeightEnum::Height), 0.0f,    1.0f, 0.0f, 0.0f,
				nstd::USC::NDC_u(500, *screenRatio, nstd::WidthHeightEnum::Width), nstd::USC::NDC_u(-500, *screenRatio, nstd::WidthHeightEnum::Height), 0.0f,     0.0f, 1.0f, 0.0f,
				nstd::USC::NDC_u(500, *screenRatio, nstd::WidthHeightEnum::Width), nstd::USC::NDC_u(500, *screenRatio, nstd::WidthHeightEnum::Height), 0.0f,    0.0f, 0.0f, 1.0f,

				nstd::USC::NDC_u(500, *screenRatio, nstd::WidthHeightEnum::Width), nstd::USC::NDC_u(500, *screenRatio, nstd::WidthHeightEnum::Height), 0.0f,    0.0f, 0.0f, 1.0f,
				nstd::USC::NDC_u(-500, *screenRatio, nstd::WidthHeightEnum::Width), nstd::USC::NDC_u(500, *screenRatio, nstd::WidthHeightEnum::Height), 0.0f,     1.0f, 1.0f, 1.0f,
				nstd::USC::NDC_u(-500, *screenRatio, nstd::WidthHeightEnum::Width), nstd::USC::NDC_u(-500, *screenRatio, nstd::WidthHeightEnum::Height), 0.0f,    1.0f, 0.0f, 0.0f
			};

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
			Data = _Data;
			// Map the vertex buffer memory
			void* mappedData = engine->get->logicalDevice->mapMemory(buffer.memory, 0, sizeof(float) * _Data.size());

			// Copy the new data directly into the mapped memory
			std::memcpy(mappedData, _Data.data(), static_cast<std::size_t>(_Data.size()));

			// Unmap the vertex buffer memory
			engine->get->logicalDevice->unmapMemory(buffer.memory);
		}
	};
}
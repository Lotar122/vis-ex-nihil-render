#pragma once
#include <iostream>
#include "Classes/Memory/Memory.hpp"

namespace nihil::graphics {
	class Engine;

	template<typename T, vk::BufferUsageFlagBits usageT = (vk::BufferUsageFlagBits)0>
	class Buffer {
	public:
		std::vector<T> Data;
		BufferData buffer;
		uint32_t bufferSize;
		Engine* engine;
		vk::BufferUsageFlagBits usage;

		Buffer(Engine* _engine, std::vector<T>& _Data, vk::BufferUsageFlagBits _usage = vk::BufferUsageFlagBits::eStorageBuffer);
		~Buffer();

		void refresh(std::vector<T> _Data);
	};
}

#include "Buffer.tpp"
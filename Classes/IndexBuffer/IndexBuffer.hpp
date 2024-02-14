#pragma once
#include <vector>
#include <iostream>
#include "Classes/Engine/Engine.hpp"
#include "Classes/Memory/Memory.hpp"
#include <vulkan/vulkan.hpp>

#include "nihil-standard/nstd.hpp"

namespace nihil {

    class IndexBuffer {
    public:
        Buffer buffer;
        uint32_t bufferSize;
        Engine* engine;
        std::vector<uint32_t> Data;

        IndexBuffer(Engine* engine)
        {
            this->engine = engine;

            void* data;
            nstd::OBJ objobj;
            std::string path = "./cube.obj";
            objobj.Load(path, engine->app->screenRatio);
            bufferSize = static_cast<uint32_t>(objobj.indicesRender.size() * sizeof(uint32_t));
            Data = objobj.indicesRender;
            // Create the index buffer
            buffer = Memory::CreateBuffer(bufferSize, vk::BufferUsageFlagBits::eIndexBuffer, engine);
            engine->get->logicalDevice->mapMemory(buffer.memory, 0, bufferSize, {}, &data);
            memcpy(data, Data.data(), static_cast<size_t>(bufferSize));
            engine->get->logicalDevice->unmapMemory(buffer.memory);
        }

        ~IndexBuffer()
        {
            // Clean up the buffer
            engine->get->logicalDevice->destroyBuffer(buffer.buffer);
            engine->get->logicalDevice->freeMemory(buffer.memory);
        }

        void refresh(const std::vector<uint32_t> indices)
        {
            bufferSize = static_cast<uint32_t>(indices.size() * sizeof(uint32_t));
            Data = indices;
            buffer = Memory::CreateBuffer(bufferSize, vk::BufferUsageFlagBits::eIndexBuffer, engine);

            // Map buffer memory
            void* data;
            engine->get->logicalDevice->mapMemory(buffer.memory, 0, bufferSize, {}, &data);

            // Copy data to the mapped memory
            memcpy(data, indices.data(), static_cast<size_t>(bufferSize));

            // Unmap buffer memory
            engine->get->logicalDevice->unmapMemory(buffer.memory);
        }
    };

}
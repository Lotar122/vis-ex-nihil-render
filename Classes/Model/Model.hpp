#pragma once

#include "nihil-standard/nstd.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <vulkan/vulkan.hpp>
#include "Classes/Buffer/Buffer.hpp"

namespace nihil::graphics {
	class Engine;

	class Model 
	{
	public:
		std::vector<float> vertices;
		std::vector<uint32_t> indices;

		glm::mat4 deafultTransform;

		Engine* engine;

		nstd::OBJ obj;

		Buffer<float, vk::BufferUsageFlagBits::eVertexBuffer>* vBuffer;
		Buffer<uint32_t, vk::BufferUsageFlagBits::eIndexBuffer>* iBuffer;

		Model(Engine* _engine, glm::mat4 _deafultTransform = glm::mat4(1.0f));

		Model(Engine* _engine, const std::string path, glm::mat4 _deafultTransform = glm::mat4(1.0f));

		Model(Engine* _engine, nstd::OBJ _obj, glm::mat4 _deafultTransform = glm::mat4(1.0f));

		void Load(const std::string path);
	};
}
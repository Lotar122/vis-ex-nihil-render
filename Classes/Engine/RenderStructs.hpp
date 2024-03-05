#pragma once
#define GLM_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED
#include <glm/glm.hpp>

namespace nihil::graphics {
	struct ObjectData {
		glm::mat4 trans;
		glm::mat4 proj;

		glm::mat4 pre;
	};
}
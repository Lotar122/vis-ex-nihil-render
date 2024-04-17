#define GRAPHICS_INCLUDES
#include <vulkan/vulkan.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#define GLM_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED

#define STD_INCLUDES
#include <iostream>
#include <vector>
#include <fstream>
#include <set>
#include <optional>

#define CUSTOM_INCLUDES
//#include "MaxwellsEngine/MaxwellsEngine.h"

#include "Classes/App/App.hpp"
#include "Classes/Engine/Engine.hpp"

#include "nihil-standard/nstd.hpp"

#include "Classes/Model/Model.hpp"
#include "Classes/InstanceData/InstanceData.hpp"
#include "Classes/Buffer/Buffer.hpp"

//Installing GLFW is a nightmare
//Installing GLFW is a nightmare
//Installing GLFW is a nightmare
//Installing GLFW is a nightmare
//Installing GLFW is a nightmare
//Installing GLFW is a nightmare
//Installing GLFW is a nightmare
//Installing GLFW is a nightmare
//Installing GLFW is a nightmare
//Installing GLFW is a nightmare
//Installing GLFW is a nightmare
//Installing GLFW is a nightmare
//Installing Vulkan is a pain
//Installing Vulkan is a pain
//Installing Vulkan is a pain
//Installing Vulkan is a pain
//Installing Vulkan is a pain
//Installing Vulkan is a pain
//Installing Vulkan is a pain
//Installing Vulkan is a pain
//Installing Vulkan is a pain
//Installing Vulkan is a pain
//Installing Vulkan is a pain
//Installing Vulkan is a pain

int main()
{
	nihil::graphics::Version vulkanVersion = {};
	vulkanVersion.make_version(0, 1, 0, 0);

	nihil::graphics::Version appVersion = {};
	appVersion.make_version(0, 1, 0, 0);

	nihil::graphics::Engine* engine = new nihil::graphics::Engine(true);

	nihil::AppCreationArgs appArgs = {};
	appArgs.appVersion = appVersion;
	appArgs.vulkanVersion = vulkanVersion;
	appArgs.engine = engine;
	appArgs.name = "vis ex nihil";
	appArgs.width = 1920;
	appArgs.height = 1080;

	nihil::App* app = new nihil::App(appArgs);

	std::thread render = std::thread([app, engine](nihil::App* app, nihil::graphics::Engine* engine) {
		engine->Setup();

		std::vector<nihil::graphics::VertexAttribute> vAttrib(8);
		vAttrib[0].binding = 0;
		vAttrib[0].location = 0;
		vAttrib[0].format = vk::Format::eR32G32B32Sfloat;
		vAttrib[0].offset = 0;

		vAttrib[1].binding = 0;
		vAttrib[1].location = 1;
		vAttrib[1].format = vk::Format::eR32G32B32Sfloat;
		vAttrib[1].offset = 3 * sizeof(float);

		vAttrib[2].binding = 0;
		vAttrib[2].location = 2;
		vAttrib[2].format = vk::Format::eR32G32Sfloat;
		vAttrib[2].offset = 6 * sizeof(float);

		vAttrib[3].binding = 0;
		vAttrib[3].location = 3;
		vAttrib[3].format = vk::Format::eR32G32B32Sfloat;
		vAttrib[3].offset = 8 * sizeof(float);


		vAttrib[4].binding = 1;
		vAttrib[4].location = 5;
		vAttrib[4].format = vk::Format::eR32G32B32A32Sfloat;
		vAttrib[4].offset = 0;

		vAttrib[5].binding = 1;
		vAttrib[5].location = 6;
		vAttrib[5].format = vk::Format::eR32G32B32A32Sfloat;
		vAttrib[5].offset = 4 * sizeof(float);

		vAttrib[6].binding = 1;
		vAttrib[6].location = 7;
		vAttrib[6].format = vk::Format::eR32G32B32A32Sfloat;
		vAttrib[6].offset = 8 * sizeof(float);

		vAttrib[7].binding = 1;
		vAttrib[7].location = 8;
		vAttrib[7].format = vk::Format::eR32G32B32A32Sfloat;
		vAttrib[7].offset = 12 * sizeof(float);

		std::vector<nihil::graphics::VertexBindingInformation> bindingInfo = {};
		nihil::graphics::VertexBindingInformation binding1 = {};
		nihil::graphics::VertexBindingInformation binding2 = {};

		binding1.inputRate = vk::VertexInputRate::eVertex;
		binding2.inputRate = vk::VertexInputRate::eInstance;

		bindingInfo.push_back(binding1);
		bindingInfo.push_back(binding2);

		vk::ShaderModule* vertexShader = NULL;
		vk::ShaderModule* fragmentShader = NULL;
		engine->renderer->CreateShaderModule("./resources/Shaders/shaderV.spv", *engine->get->logicalDevice, &vertexShader);
		engine->renderer->CreateShaderModule("./resources/Shaders/shaderF.spv", *engine->get->logicalDevice, &fragmentShader);

		nihil::graphics::PipelineInfo pipelineInfo = engine->CreatePipelineConfiguration(vAttrib, bindingInfo, vertexShader, fragmentShader);
		nihil::graphics::PipelineAndRenderPass par = engine->CreatePipeline(pipelineInfo);

		glm::mat4 trans = glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		std::vector<float> instanceData(0);

		for (int i = 0; i < 4; i++)
		{
			glm::mat4 y = glm::translate(glm::mat4(1.0f), glm::vec3(10.0f * i, 0.0f, 0.0f));
			std::vector<float> x = nihil::nstd::flattenMatrix4x4(y);

			for (float& f : x)
			{
				instanceData.push_back(f);
			}
		}

		nihil::graphics::Buffer<float, vk::BufferUsageFlagBits::eVertexBuffer> instanceBuffer(engine, instanceData);

		nihil::graphics::Model modelCar(engine, "./resources/models/cube.obj", &instanceBuffer);
		nihil::nstd::ComponentData dataCar = {};
		dataCar.any = (void*)&modelCar;
		nihil::nstd::Component componentCar(dataCar, nihil::nstd::ComponentType::any, sizeof(modelCar), &typeid(modelCar));

		std::vector<nihil::nstd::Component> compArr;
		compArr.push_back(componentCar);

		//pipeline assignment
		engine->renderer->pipeline = par.pipeline;
		engine->renderer->renderPass = par.renderPass;

		while (!*(app->get->shouldClose))
		{
			engine->Draw(compArr);
		}
	}, app, engine);

	while (!*(app->get->shouldClose))
	{
		app->handle();
	}
	
	render.join();

	//error causing (intentional) 
	//std::cout << *((int*)nullptr) << std::endl;
	//i read from a nullptr to cause a read access violation error

	return 0;
}
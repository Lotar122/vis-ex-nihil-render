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

		nihil::graphics::Model modelCar(engine, "./resources/models/lambo.obj", glm::translate(glm::mat4(1.0f), glm::vec3(-5.0f, 0.0f, 0.0f)));
		nihil::nstd::ComponentData dataCar = {};
		dataCar.any = (void*)&modelCar;
		nihil::nstd::Component componentCar(dataCar, nihil::nstd::ComponentType::any, sizeof(modelCar), &typeid(modelCar));

		nihil::graphics::Model modelCube(engine, "./resources/models/car.obj", glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(5.0f, 0.0f, 0.0f)), glm::vec3(0.1f, 0.1f, 0.1f)));
		nihil::nstd::ComponentData dataCube = {};
		dataCube.any = (void*)&modelCube;
		nihil::nstd::Component componentCube(dataCube, nihil::nstd::ComponentType::any, sizeof(modelCube), &typeid(modelCube));

		std::vector<nihil::nstd::Component> compArr;
		compArr.push_back(componentCar);
		compArr.push_back(componentCube);

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
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

#include "App/App.hpp"

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
	//MaxwellsEngine* engine = new MaxwellsEngine(true);
	//engine->Setup();

	//while (!engine->shouldClose)
	//{
	//	engine->WindowEventLoop();
	//	engine->Draw();
	//}

	//delete engine;

	nihil::Version vulkanVersion = {};
	vulkanVersion.make_version(0, 1, 0, 0);

	nihil::Version appVersion = {};
	appVersion.make_version(0, 1, 0, 0);

	nihil::Engine* engine = new nihil::Engine(true);

	nihil::AppCreationArgs appArgs = {};
	appArgs.appVersion = appVersion;
	appArgs.vulkanVersion = vulkanVersion;
	appArgs.engine = engine;
	appArgs.name = "vis ex nihil";
	appArgs.width = 1920;
	appArgs.height = 1080;

	nihil::App* app = new nihil::App(appArgs);

	//engine->Setup();
	engine->Setup();

	std::thread render = std::thread([app, engine](nihil::App* appP, nihil::Engine* engineP) {
		while (!*(appP->get->shouldClose))
		{
			appP->get->engine->Draw(appP->get->scene);
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
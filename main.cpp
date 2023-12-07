#define GRAPHICS_INCLUDES
#include <vulkan/vulkan.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define STD_INCLUDES
#include <iostream>
#include <vector>
#include <fstream>
#include <set>
#include <optional>

#define CUSTOM_INCLUDES
//#include "MaxwellsEngine/MaxwellsEngine.h"

#include "App/App.h"

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

struct shaderInfo {
	int num;
	std::string text;
	bool boolean;
};

namespace ComponentType {
	enum ComponentType {
		Shader
	};
}

struct ComponentInfo {
	void* ptr;
	ComponentType::ComponentType type;
};

class Component
{
public:
	void* ptr;
	ComponentType::ComponentType type;

	Component(ComponentType::ComponentType _type, void* _ptr)
	{
		ptr = _ptr;
		type = _type;
	}

	void* getRawPtr()
	{
		return ptr;
	}
	ComponentInfo getPtrWithType()
	{
		ComponentInfo compInfo = {};
		compInfo.ptr = ptr;
		compInfo.type = type;
	}
};
class ComponentArray
{
public:
	std::vector<Component*> compArr;

	void AttachComponent(Component* comp)
	{
		compArr.push_back(comp);
	}

	void DeleteComponent(uint32_t index)
	{
		compArr.erase(compArr.begin() + index);
	}

	void* getRawPtr(uint32_t index)
	{
		return compArr[index]->ptr;
	}
};



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



	Engine* engine = new Engine(true);

	App* app = new App(1920, 1080, "Vulkan", engine);

	shaderInfo info = {};
	info.num = 10;
	info.boolean = true;
	info.text = "q0we";

	Component* comp = new Component(ComponentType::Shader, &info);

	ComponentArray compArr = ComponentArray();
	compArr.AttachComponent(comp);

	shaderInfo* inf = (shaderInfo*)compArr.getRawPtr(0);
	std::cout << inf->text << std::endl;

	while (!app->shouldClose)
	{
		app->handle();
		app->engine->Draw(app->scene);
	}

	//getchar();
	//getchar();

	return 0;
}
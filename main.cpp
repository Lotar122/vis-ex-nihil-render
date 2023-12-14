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


void displayBits(uint16_t value) {
	const int bits = 8 * sizeof(uint16_t);
	for (int i = bits - 1; i >= 0; --i) {
		uint16_t mask = 1 << i;
		std::cout << ((value & mask) ? '1' : '0');

		// Add a space for better readability
		if (i % 4 == 0) {
			std::cout << ' ';
		}
	}
	std::cout << std::endl;
}

void setBit(uint8_t& number, int bitPosition) {
	// Set the bit at the specified position to 1
	number |= (1 << bitPosition);
}

void clearBit(uint8_t& number, int bitPosition) {
	// Clear the bit at the specified position to 0
	number &= ~(1 << bitPosition);
}

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

	engine->Setup();

	shaderInfo info = {};
	info.num = 10;
	info.boolean = true;
	info.text = "q0we";

	Component* comp = new Component(ComponentType::Shader, &info);

	ComponentArray compArr = ComponentArray();
	compArr.AttachComponent(comp);

	shaderInfo* inf = (shaderInfo*)compArr.getRawPtr(0);
	//std::cout << inf->text << std::endl;

	while (!*(app->get->shouldClose))
	{
		app->handle();
		app->get->engine->Draw(app->get->scene);
	}

	//error causing (intentional) 
	//std::cout << *((int*)nullptr) << std::endl;
	//i read from a nullptr to cause a read access vilation error

	//getchar();
	//getchar();

	return 0;
}
#include "App.h"

App::App(uint32_t width, uint32_t height, std::string name, Engine* _engine)
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);

	engine = _engine;
	engine->setApp(this);
	engine->Setup();

	scene = new Scene();
}
App::~App()
{
	glfwDestroyWindow(window);
	glfwTerminate();

	delete engine;
	delete scene;
}
void App::handle()
{
	shouldClose = glfwWindowShouldClose(window);
	glfwPollEvents();
	engine->Draw(scene);
}
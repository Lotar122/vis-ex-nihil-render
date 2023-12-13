#include "App.hpp"

App::App(uint32_t width, uint32_t height, std::string name, Engine* _engine)
{
	glfwInit();
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

	scene = new Scene();

	get = new Proxy(
		&width,
		&height,
		&name,
		_engine,
		scene,
		window,
		&shouldClose
	);

	window = glfwCreateWindow(width, height, name.c_str(), NULL, NULL);

	get->engine = _engine;
	get->scene = scene;
	get->window = window;

	engine = _engine;
	engine->setApp(this);
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
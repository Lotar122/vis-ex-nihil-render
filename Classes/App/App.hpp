#pragma once
#include <iostream>
#include <GLFW/glfw3.h>
#include "Scene/Scene.h"

class Engine;

class App
{
public:
	uint32_t width;
	uint32_t height;
	std::string name;

	Engine* engine;
	Scene* scene;

	GLFWwindow* window;

	bool shouldClose = false;

	App(uint32_t width, uint32_t height, std::string name, Engine* engine);
	~App();

	void handle();
};

#include "Engine/Engine.h"
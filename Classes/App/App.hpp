#pragma once
#include <iostream>
#include <GLFW/glfw3.h>
#include "Classes/Scene/Scene.hpp"

namespace nihil {
	class Engine;

	class App
	{
	private:
		struct Proxy {
			const uint32_t* width;
			const uint32_t* height;
			const std::string* name;
			Engine* engine;
			Scene* scene;
			GLFWwindow* window;
			const bool* shouldClose;
			Proxy(
				const uint32_t* width,
				const uint32_t* height,
				const std::string* name,
				Engine* engine,
				Scene* scene,
				GLFWwindow* window,
				const bool* shouldClose
			)
			{
				this->width = width;
				this->height = height;
				this->name = name;
				this->engine = engine;
				this->scene = scene;
				this->window = window;
				this->shouldClose = shouldClose;
			}
		};
		uint32_t width;
		uint32_t height;
		std::string name;

		Engine* engine;
		Scene* scene;

		GLFWwindow* window;

		bool shouldClose = false;

	public:
		Proxy* get;
		App(uint32_t width, uint32_t height, std::string name, Engine* engine);
		~App();

		void handle();

		inline void SetWidth(uint32_t width) {
			this->width = width;
		}
		inline void SetHeight(uint32_t height) {
			this->height = height;
		}
	};
}

#include "Engine/Engine.hpp"
#pragma once
#include <iostream>
#include <GLFW/glfw3.h>
#include "Classes/Scene/Scene.hpp"
#include "Classes/Engine/Engine.Structs.InData.hpp"

namespace nihil {
	class Engine;

	struct AppCreationArgs {
		uint16_t width;
		uint16_t height;
		std::string name;
		Version vulkanVersion;
		Version appVersion;
		Engine* engine;
	};

	class App
	{
	private:
		struct Proxy {
			const uint16_t* width;
			const uint16_t* height;
			const std::string* name;
			Engine* engine;
			Scene* scene;
			GLFWwindow* window;
			const bool* shouldClose;
			const Version* vulkanVersion;
			const Version* appVersion;
			Proxy(
				const uint16_t* width,
				const uint16_t* height,
				const std::string* name,
				Engine* engine,
				Scene* scene,
				GLFWwindow* window,
				const bool* shouldClose,
				const Version* appVersion,
				const Version* vulkanVersion
			)
			{
				this->width = width;
				this->height = height;
				this->name = name;
				this->engine = engine;
				this->scene = scene;
				this->window = window;
				this->shouldClose = shouldClose;
				this->appVersion = appVersion;
				this->vulkanVersion = vulkanVersion;
			}
		};
		uint16_t width;
		uint16_t height;
		std::string name;

		Engine* engine;
		Scene* scene;

		GLFWwindow* window;

		Version vulkanVersion;
		Version appVersion;

		bool shouldClose = false;

	public:
		Proxy* get;
		App(AppCreationArgs args);
		~App();

		void handle();

		inline void SetWidth(uint16_t width) {
			this->width = width;
		}
		inline void SetHeight(uint16_t height) {
			this->height = height;
		}
	};
}

#include "Engine/Engine.hpp"
#pragma once
#define GRAPHICS_INCLUDES
#include <vulkan/vulkan.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <set>
#include <optional>
#include <vector>
#include "implicit_cast.hpp"
#include "Classes/SPIRV/SPIRV.hpp"

//!!! Enable colors
#define USE_COLORS
#include "TerminalColors.hpp"

#include "Classes/App/App.hpp"
#include "Engine.Structs.InData.hpp"

#include "RenderStructs.hpp"

#include "Classes/Scene/Scene.hpp"

#define GLM_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_LEFT_HANDED

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Classes/Renderer/Renderer.hpp"
#include "Classes/Buffer/Buffer.hpp"

namespace nihil::graphics {

	struct EngineCreateInfo {

	};

	class Engine
	{
		friend class Renderer;
	private:
		struct Proxy {
			const vk::Instance* instance;
			const vk::PhysicalDevice* physicalDevice;
			const vk::Device* logicalDevice;

			Proxy(
				const vk::Instance* instance,
				const vk::PhysicalDevice* physicalDevice,
				const vk::Device* logicalDevice
			)
			{
				this->instance = instance;
				this->logicalDevice = logicalDevice;
				this->physicalDevice = physicalDevice;
			}
		};

		Renderer* renderer;
	public:
		App* app = NULL;
		bool shouldClose = false;
		bool debug = false;

		Engine(bool _debug);
		Engine();
		~Engine();

		void setApp(App* _app);

		//Setup (for now just calls setupdeafult)
		void Setup();

		Proxy* get;

		nstd::OBJ objobject;

		void Draw(std::vector<nstd::Component>& modelArr);

		//allow buffer operations
		void writeindexBuffer(std::vector<uint32_t> data);

		void writeVertexBuffer(std::vector<float> data);

		std::vector<uint32_t> readIndexBuffer();

		std::vector<float> readVertexBuffer();

	private:
		/*
		* @brief Creates a Vulkan Instance
		*
		* @param createInfo - Information for the creation of a vulkan instance
		* @return None
		*/
		void CreateVulkanInstance(VulkanInstanceCreateInfo createInfo);
		/*
		* @brief Picks the Device for you
		*
		* @param None
		* @return None
		*/
		void PickPhysicalDevice();
		/*
		* @brief Creates a Vulkan Surface
		*
		* @param window - a pointer to a GLFW window thats used by your application
		* @return None
		*/
		void CreateVulkanSurface(GLFWwindow* window);
		/*
		* @brief Creates a Vulkan logical dedvice
		*
		* @param None
		* @return None
		*/
		void CreateVulkanLogicalDevice();
		/*
		* @brief Creates the Present Queue and the Render Queue
		*
		* @param None
		* @return None
		*/
		void CreateVulkanQueues();

		vk::Instance instance;
		vk::PhysicalDevice device;
		vk::SurfaceKHR surface;
		QueueFamilyIndices famind;
		std::vector<uint32_t> uniqueFAMIND;
		vk::Device logicalDevice;

		vk::Queue graphicsQueue;
		vk::Queue presentQueue;

		bool error = false;

		inline void finishSetup()
		{
			if (error)
			{
				std::cerr << std::endl << YELLOW << "[Setup]" << RESET << "Engine setup summary: " << RED << "[###]" << RESET << std::endl << std::endl;
				std::cerr << BLUE << "[====================================================================================================================]" << RESET << std::endl << std::endl;
			}
			else
			{
				std::cout << std::endl << YELLOW << "[Setup]" << RESET << "Engine setup summary: " << GREEN << "[###]" << RESET << std::endl << std::endl;
				std::cout << BLUE << "[====================================================================================================================]" << RESET << std::endl << std::endl;
			}
		}
		inline void finishPipelineSetup()
		{
			if (error)
			{
				std::cerr << std::endl << YELLOW << "[Setup]" << MAGENTA << "->" << YELLOW << "[Pipeline]" << RESET << "Pipeline setup summary: " << RED << "[###]" << RESET << std::endl << std::endl;
				std::cerr << BLUE << "[====================================================================================================================]" << RESET << std::endl << std::endl;
			}
			else
			{
				std::cout << std::endl << YELLOW << "[Setup]" << MAGENTA << "->" << YELLOW << "[Pipeline]" << RESET << "Pipeline setup summary: " << GREEN << "[###]" << RESET << std::endl << std::endl;
				std::cout << BLUE << "[====================================================================================================================]" << RESET << std::endl << std::endl;
			}
		}
		inline void finishPrimarySetup()
		{
			if (error)
			{
				std::cerr << std::endl << YELLOW << "[Setup]" << RESET << "Primary Engine setup summary: " << RED << "[###]" << RESET << std::endl << std::endl;
				std::cerr << BLUE << "[====================================================================================================================]" << RESET << std::endl << std::endl;
			}
			else
			{
				std::cout << std::endl << YELLOW << "[Setup]" << RESET << "Primary Engine setup summary: " << GREEN << "[###]" << RESET << std::endl << std::endl;
				std::cout << BLUE << "[====================================================================================================================]" << RESET << std::endl << std::endl;
			}
		}

		inline bool isSuitable(const vk::PhysicalDevice& device)
		{
			if (!(device.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu)) return false;
			const std::vector<const char*> requestedExtensions = {
				VK_KHR_SWAPCHAIN_EXTENSION_NAME
			};
			std::set<std::string> requiredExtensions(requestedExtensions.begin(), requestedExtensions.end());
			for (const vk::ExtensionProperties& extension : device.enumerateDeviceExtensionProperties())
			{
				requiredExtensions.erase(extension.extensionName);
			}
			return requiredExtensions.empty();
		}
	};
};
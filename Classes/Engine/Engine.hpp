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

#define USE_COLORS
#include "TerminalColors.hpp"

#include "Classes/App/App.hpp"
#include "Engine.Structs.InData.hpp"

#include "RenderStructs.hpp"

#include "Classes/Scene/Scene.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace nihil {
	//instead of #include "VertexBuffer/VertexBuffer.h"
	class VertexBuffer;

	class Engine
	{
	private:
		struct Proxy {
			const vk::Instance* instance;
			const vk::PhysicalDevice* physicalDevice;
			const vk::Device* logicalDevice;
			const vk::RenderPass* renderPass;
			const vk::Pipeline* pipeline;
			const vk::CommandPool* commandPool;
			const vk::CommandBuffer* commandBuffer;
			const SwapChainBundle* swapchainBundle;

			Proxy(
				const vk::Instance* instance,
				const vk::PhysicalDevice* physicalDevice,
				const vk::Device* logicalDevice,
				const vk::RenderPass* renderPass,
				const vk::Pipeline* pipeline,
				const vk::CommandPool* commandPool,
				const vk::CommandBuffer* commandBuffer,
				const SwapChainBundle* swapchainBundle
			)
			{
				this->commandBuffer = commandBuffer;
				this->commandPool = commandPool;
				this->instance = instance;
				this->logicalDevice = logicalDevice;
				this->physicalDevice = physicalDevice;
				this->pipeline = pipeline;
				this->renderPass = renderPass;
				this->swapchainBundle = swapchainBundle;
			}
		};
	public:
		App* app = NULL;
		bool shouldClose = false;
		bool debug = false;

		int FPS = 80;
		std::chrono::microseconds frameDuration = std::chrono::microseconds((int)1000000 / (int)FPS);
		std::chrono::microseconds correction;

		Engine(bool _debug);
		Engine();
		~Engine();

		void setApp(App* _app);

		//Setup (for now just calls setupdeafult)
		void Setup();

		//creates the deafult engine to render a simple white triangle
		void SetupDeafult();

		//Draw
		void Draw(Scene* scene);

		Proxy* get;

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
		/*
		* @brief Creates a swapchain
		*
		* @param createInfo - config creation info
		* @return inforamtion to create the swapchain
		*/
		SwapchainConfiguration CreateSwapchainConfiguration(SwapchainConfigCreateInfo createInfo);
		/*
		* @brief Creates a swapchain
		*
		* @param createInfo - information for the creation of the swapchain
		* @return None
		*/
		void CreateSwapchain(SwapchainConfiguration createInfo);
		/*
		* @brief Creates a swapchain
		*
		* @param None
		* @return None
		*/
		void CreateImageViews();
		//sets up the basic pipeline
		void PipelineSetup();
		//creates the shader module
		void CreateShaderModule(std::string filepath, vk::Device device, vk::ShaderModule** ppShaderModule);
		//render setup related code (organize in future)
		void RenderSetup();
		//records draw commands
		void recordDrawCommands(vk::CommandBuffer& commandBuffer, uint32_t imageIndex, Scene* scene);
		//destroy the swapchain
		void destroySwapchain();
		//create the flow-control
		void createSyncObjects();
		//create framebuffers
		void createFrameBuffers();
		//make frame commandbuffers
		void createFrameCommandBuffers();
		//make main commandbuffer
		void createMainCommandBuffer();
		//make commandpools
		void createCommandPools();
		//reacreate swapchain
		void RecreateSwapchain();

		vk::Instance instance;
		vk::PhysicalDevice device;
		vk::SurfaceKHR surface;
		QueueFamilyIndices famind;
		std::vector<uint32_t> uniqueFAMIND;
		vk::Device logicalDevice;

		vk::Queue graphicsQueue;
		vk::Queue presentQueue;

		SwapChainSupportDetails support;
		vk::SurfaceFormatKHR surfaceFormat;
		vk::PresentModeKHR presentMode;
		vk::SwapchainCreateInfoKHR swapCreateInfo;

		SwapChainBundle swapchainBundle{};

		uint32_t imageCount;

		vk::ShaderModule* vertexShader = NULL;
		vk::ShaderModule* fragmentShader = NULL;

		vk::PipelineLayout layout;
		vk::RenderPass renderPass;
		vk::Pipeline pipeline;

		vk::CommandPool commandPool;
		vk::CommandBuffer commandBuffer;

		uint32_t maxFramesInFlight, frameNumber;

		//VertexBuffer
		VertexBuffer* vertexBuffer;

		SwapchainConfiguration swapchainConfiguration;

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
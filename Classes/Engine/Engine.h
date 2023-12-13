#pragma once
#define GRAPHICS_INCLUDES
#include <vulkan/vulkan.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <iostream>
#include <set>
#include <optional>
#include <vector>
#include "implicit_cast.h"
#include "SPIRV/SPIRV.h"

#define USE_COLORS
#include "TerminalColors.h"

#include "App/App.h"
#include "Engine.Structs.InData.h"

#include "RenderStructs.h"

#include "Scene/Scene.h"

#include "Classes/ClassApi.h"

//instead of #include "VertexBuffer/VertexBuffer.h"
class VertexBuffer;

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Engine
{
public:
	App* app = NULL;
	bool shouldClose = false;
	bool debug = false;

	int FPS = 60;
	std::chrono::milliseconds period = std::chrono::milliseconds(static_cast<long long>((float)1000 / (float)60));

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

	inline const vk::Device* GetLogicalDevice() {
		return &logicalDevice;
	}
	inline const vk::PhysicalDevice* GetPhysicalDevice() {
		return &device;
	}
	EngineGet* get;

private:
	//picks the right device
	void PickDevice();
	//creates a Vulkan instance
	void CreateVulkanInstance(InstanceCreateInfo& info);
	//creates a Vulkan surface
	void CreateSurface();
	//creates the required queues
	void CreateQueues();
	//creates a Vulkan logical device
	void CreateLogicDevice();
	//generates the swapchain configuration
	void PreConfigSwapchain();
	//creates a Vulkan swapchain
	void CreateSwapchain();
	//creates views to the images of the swapchain
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
	vk::Extent2D extent;
	vk::SwapchainCreateInfoKHR swapCreateInfo;

	SwapChainBundle bundle{};

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
		const std::vector<const char*> requestedExtensions = {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		};
		std::set<std::string> requiredExtensions(requestedExtensions.begin(), requestedExtensions.end());
		for (const vk::ExtensionProperties& extension : device.enumerateDeviceExtensionProperties())
		{
			requiredExtensions.erase(extension.extensionName);
		}
		return requiredExtensions.empty() && device.getProperties().deviceType == vk::PhysicalDeviceType::eDiscreteGpu;
	}
};
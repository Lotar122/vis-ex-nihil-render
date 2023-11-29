#pragma once
#define GRAPHICS_INCLUDES
#include <vulkan/vulkan.hpp>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#define COMMMON_INCLUDES
#include <iostream>
#include <set>
#include <optional>
#include <vector>
#include "implicit_cast.h"
#include "SPIRV/SPIRV.h"

//#define USE_COLORS
#include "TerminalColors.h";

struct WindowData {
	int width;
	int height;
	std::string name;
};
struct QueueFamilyIndices {
	std::optional<uint32_t> graphics;
	std::optional<uint32_t> present;

	bool isComplete()
	{
		return graphics.has_value() && present.has_value();
	}
};
struct SwapChainSupportDetails {
	vk::SurfaceCapabilitiesKHR capabilities;
	std::vector<vk::SurfaceFormatKHR> formats;
	std::vector<vk::PresentModeKHR> presentModes;
};
struct SwapChainFrame {
	vk::Image image;
	vk::ImageView view;
	vk::Framebuffer frameBuffer;
	vk::CommandBuffer commandBuffer;
};
struct SwapChainBundle {
	vk::SwapchainKHR swapchain;
	std::vector<SwapChainFrame> frames;
	vk::Format format;
	vk::Extent2D extent;
};
struct PipelineInBundle {

};
struct PipelineOutBundle {

};

//the Engine class - the super-class that manages Vulkan related stuff
class MaxwellsEngine
{
public:
	bool shouldClose = false;
	bool debug = false;

	MaxwellsEngine();
	MaxwellsEngine(bool _debug);
	~MaxwellsEngine();

	//picks the right device
	void PickDevice();
	//creates the window
	void CreateWindow();
	//creates a Vulkan instance
	void CreateVulkanInstance();
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

	//runs all functions required for minimal setup for you!!!
	void Setup();

	//render setup related code (organize in future)
	void RenderSetup();

	//call this in the event (message) loop
	void WindowEventLoop();

	//Draw
	void Draw();

	//creates the shader module
	void CreateShaderModule(std::string filepath, vk::Device device, vk::ShaderModule** ppShaderModule);
private:
	GLFWwindow* window;
	WindowData winData;
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

	vk::Semaphore imageAvailable, renderFinished;
	vk::Fence inFlightFence;

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

	void recordDrawCommands(vk::CommandBuffer& commandBuffer, uint32_t imageIndex);

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
		return requiredExtensions.empty();
	}
};
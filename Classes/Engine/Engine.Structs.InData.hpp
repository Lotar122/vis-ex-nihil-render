#pragma once
#include <iostream>
#include <vector>
#include <iostream>
#include <set>
#include <optional>
#include <vulkan/vulkan.hpp>

namespace nihil {
	using SwapchainConfiguration = vk::SwapchainCreateInfoKHR;

	struct Version {
		uint32_t variant;
		uint32_t major;
		uint32_t minor;
		uint32_t patch;

		void make_version(uint32_t _variant, uint32_t _major, uint32_t _minor, uint32_t _patch)
		{
			variant = _variant;
			major = _major;
			minor = _minor;
			patch = _patch;
		}
	};
	struct VulkanInstanceCreateInfo {
		Version appVersion;
		Version vulkanVersion;
		std::string appName;
		bool validationLayers = false;
	};

	enum class BufferingMode {
		eSingle = 1,
		eDouble = 2,
		eTriple = 3,
		eQuadruple = 4
	};

	struct SwapchainConfigCreateInfo {
		uint16_t windowWidth;
		uint16_t windowHeight;
		BufferingMode preferredBuffering;
	};

	struct SwapchainSupportDetails {
		vk::SurfaceCapabilitiesKHR capabilities;
		std::vector<vk::SurfaceFormatKHR> formats;
		std::vector<vk::PresentModeKHR> presentModes;
	};

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

		vk::Semaphore imageAvailable, renderFinished;
		vk::Fence inFlightFence;
	};
	struct SwapChainBundle {
		vk::SwapchainKHR swapchain;
		std::vector<SwapChainFrame> frames;
		vk::Format format;
		vk::Extent2D extent;
	};
}
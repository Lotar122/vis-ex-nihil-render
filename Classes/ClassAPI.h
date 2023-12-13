#pragma once
#include <vulkan/vulkan.hpp>
#include "Engine/Engine.Structs.InData.h"
struct EngineGet {
	const vk::Instance* instance;
	const vk::PhysicalDevice* physicalDevice;
	const vk::Device* logicalDevice;
	const vk::RenderPass* renderPass;
	const vk::Pipeline* pipeline;
	const vk::CommandPool* commandPool;
	const vk::CommandBuffer* commandBuffer;
	const SwapChainBundle* swapchainBundle;
};
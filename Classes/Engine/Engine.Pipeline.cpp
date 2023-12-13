#include "Engine.h"

void Engine::CreateShaderModule(std::string filepath, vk::Device device, vk::ShaderModule** ppShaderModule)
{
	std::cout << YELLOW << "[Setup]" << MAGENTA << "->" << YELLOW << "[Pipeline]" << MAGENTA << "->" << YELLOW << "[Create-Shader]" << RESET << "Creating shader module ";
	std::vector<char> sourceCode = SPIRV::LoadFile(filepath);
	vk::ShaderModuleCreateInfo createInfo = {};
	createInfo.flags = vk::ShaderModuleCreateFlags();
	createInfo.codeSize = sourceCode.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(sourceCode.data());

	vk::ShaderModule module;

	try {
		module = device.createShaderModule(createInfo);
	}
	catch (vk::SystemError err) {
		std::cerr << RED << "[###]" << RESET << std::endl;
		error = true;
		finishPipelineSetup();
		finishSetup();
		std::abort();
	}
	std::cout << GREEN << "[###]" << RESET << std::endl;

	*ppShaderModule = &module;
}

void Engine::PipelineSetup()
{
	vk::GraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.flags = vk::PipelineCreateFlags();
	std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;

	if (debug) std::cout << YELLOW << "[Setup]" << MAGENTA << "->" << YELLOW << "[Pipeline]" << RESET << "Creating a vertex input " << GREEN << "[###]" << RESET << std::endl;
	//Vertex Input
	vk::VertexInputBindingDescription bindingDesc = {};
	bindingDesc.binding = 0;
	bindingDesc.stride = 6 * sizeof(float);
	bindingDesc.inputRate = vk::VertexInputRate::eVertex;

	std::array<vk::VertexInputAttributeDescription, 2> attributes;
	attributes[0].binding = 0;
	attributes[0].location = 0;
	attributes[0].format = vk::Format::eR32G32B32Sfloat;
	attributes[0].offset = 0;

	attributes[1].binding = 0;
	attributes[1].location = 1;
	attributes[1].format = vk::Format::eR32G32B32Sfloat;
	attributes[1].offset = 3 * sizeof(float);

	vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.flags = vk::PipelineVertexInputStateCreateFlags();
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.pVertexBindingDescriptions = &bindingDesc;
	vertexInputInfo.vertexAttributeDescriptionCount = 2;
	vertexInputInfo.pVertexAttributeDescriptions = attributes.data();
	pipelineInfo.pVertexInputState = &vertexInputInfo;

	if (debug) std::cout << YELLOW << "[Setup]" << MAGENTA << "->" << YELLOW << "[Pipeline]" << RESET << "Creating a input assembly " << GREEN << "[###]" << RESET << std::endl;
	//Input Assembly
	vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
	inputAssemblyInfo.flags = vk::PipelineInputAssemblyStateCreateFlags();
	inputAssemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;
	pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;

	//Vertex shader
	// in class declaration: vk::ShaderModule* vertexShader = NULL;
	CreateShaderModule("./Shaders/shaderV.spv", logicalDevice, &vertexShader);
	vk::PipelineShaderStageCreateInfo vertexShaderInfo = {};
	vertexShaderInfo.flags = vk::PipelineShaderStageCreateFlags();
	vertexShaderInfo.stage = vk::ShaderStageFlagBits::eVertex;
	vertexShaderInfo.module = *vertexShader;
	vertexShaderInfo.pName = "main";
	shaderStages.push_back(vertexShaderInfo);

	if (debug) std::cout << YELLOW << "[Setup]" << MAGENTA << "->" << YELLOW << "[Pipeline]" << RESET << "Creating the viewport " << GREEN << "[###]" << RESET << std::endl;
	//Viewport
	vk::Viewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = extent.width;
	viewport.height = extent.height;
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;
	vk::Rect2D scissor = {};
	scissor.offset.x = 0.0f;
	scissor.offset.y = 0.0f;
	scissor.extent = extent;
	vk::PipelineViewportStateCreateInfo viewportInfo = {};
	viewportInfo.flags = vk::PipelineViewportStateCreateFlags();
	viewportInfo.viewportCount = 1;
	viewportInfo.pViewports = &viewport;
	viewportInfo.scissorCount = 1;
	viewportInfo.pScissors = &scissor;
	pipelineInfo.pViewportState = &viewportInfo;

	if (debug) std::cout << YELLOW << "[Setup]" << MAGENTA << "->" << YELLOW << "[Pipeline]" << RESET << "Creating a rasterizer " << GREEN << "[###]" << RESET << std::endl;
	//Rasterization
	vk::PipelineRasterizationStateCreateInfo rasterizationInfo = {};
	rasterizationInfo.flags = vk::PipelineRasterizationStateCreateFlags();
	rasterizationInfo.depthClampEnable = VK_FALSE;
	rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterizationInfo.polygonMode = vk::PolygonMode::eFill;
	rasterizationInfo.lineWidth = 1.0f;
	rasterizationInfo.cullMode = vk::CullModeFlagBits::eBack;
	rasterizationInfo.frontFace = vk::FrontFace::eClockwise;
	rasterizationInfo.depthBiasEnable = VK_FALSE;
	pipelineInfo.pRasterizationState = &rasterizationInfo;

	//Fragment shader
	//in class declaration: vk::ShaderModule* fragmentShader = NULL;
	CreateShaderModule("./Shaders/shaderF.spv", logicalDevice, &fragmentShader);
	vk::PipelineShaderStageCreateInfo fragmentShaderInfo = {};
	fragmentShaderInfo.flags = vk::PipelineShaderStageCreateFlags();
	fragmentShaderInfo.stage = vk::ShaderStageFlagBits::eFragment;
	fragmentShaderInfo.module = *fragmentShader;
	fragmentShaderInfo.pName = "main";
	shaderStages.push_back(fragmentShaderInfo);

	if (debug) std::cout << YELLOW << "[Setup]" << MAGENTA << "->" << YELLOW << "[Pipeline]" << RESET << "Setting shader stages " << GREEN << "[###]" << RESET << std::endl;
	//Set shaders
	pipelineInfo.stageCount = shaderStages.size();
	pipelineInfo.pStages = shaderStages.data();

	if (debug) std::cout << YELLOW << "[Setup]" << MAGENTA << "->" << YELLOW << "[Pipeline]" << RESET << "Creating a multisampler " << GREEN << "[###]" << RESET << std::endl;
	//Multisampling
	vk::PipelineMultisampleStateCreateInfo multisampleInfo = {};
	multisampleInfo.flags = vk::PipelineMultisampleStateCreateFlags();
	multisampleInfo.sampleShadingEnable = VK_FALSE;
	multisampleInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;
	pipelineInfo.pMultisampleState = &multisampleInfo;

	if (debug) std::cout << YELLOW << "[Setup]" << MAGENTA << "->" << YELLOW << "[Pipeline]" << RESET << "Specifying a color blender " << GREEN << "[###]" << RESET << std::endl;
	//Color blend
	vk::PipelineColorBlendAttachmentState attachment = {};
	attachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
	attachment.blendEnable = VK_FALSE;
	vk::PipelineColorBlendStateCreateInfo colorBlendingInfo = {};
	colorBlendingInfo.flags = vk::PipelineColorBlendStateCreateFlags();
	colorBlendingInfo.logicOpEnable = VK_FALSE;
	colorBlendingInfo.logicOp = vk::LogicOp::eCopy;
	colorBlendingInfo.attachmentCount = 1;
	colorBlendingInfo.pAttachments = &attachment;
	colorBlendingInfo.blendConstants[0] = 0.0f;
	colorBlendingInfo.blendConstants[1] = 0.0f;
	colorBlendingInfo.blendConstants[2] = 0.0f;
	colorBlendingInfo.blendConstants[3] = 0.0f;
	pipelineInfo.pColorBlendState = &colorBlendingInfo;



	if (debug) std::cout << YELLOW << "[Setup]" << MAGENTA << "->" << YELLOW << "[Pipeline]" << RESET << "Creating a pipeline layout ";
	//Create pipeline layout (move to a seperate function in future)
	//in class declaration: vk::PipelineLayout layout;
	vk::PipelineLayoutCreateInfo layoutInfo = {};
	layoutInfo.flags = vk::PipelineLayoutCreateFlags();
	layoutInfo.setLayoutCount = 0;
	layoutInfo.pushConstantRangeCount = 1;

	vk::PushConstantRange pushConstantInfo = {};
	pushConstantInfo.offset = 0;
	pushConstantInfo.size = sizeof(ObjectData);
	pushConstantInfo.stageFlags = vk::ShaderStageFlagBits::eVertex;

	layoutInfo.pPushConstantRanges = &pushConstantInfo;

	try {
		layout = logicalDevice.createPipelineLayout(layoutInfo);
	}
	catch (vk::SystemError) {
		std::cout << RED << "[###]" << RESET << std::endl;
		error = true;
		finishPipelineSetup();
		std::abort();
	}
	std::cout << GREEN << "[###]" << RESET << std::endl;

	if (debug) std::cout << YELLOW << "[Setup]" << MAGENTA << "->" << YELLOW << "[Pipeline]" << RESET << "Creating a renderpass ";
	//Create render pass (move to a seperate function in future)
	//in class declaration: vk::RenderPass renderPass;
	vk::AttachmentDescription colorAttachment = {};
	colorAttachment.flags = vk::AttachmentDescriptionFlags();
	colorAttachment.format = bundle.format;
	colorAttachment.samples = vk::SampleCountFlagBits::e1;
	colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
	colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
	colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
	colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
	colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
	colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

	vk::AttachmentReference colorAttachmentRef = {};
	colorAttachmentRef.attachment = 0;
	colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

	vk::SubpassDescription subpass = {};
	subpass.flags = vk::SubpassDescriptionFlags();
	subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttachmentRef;

	vk::RenderPassCreateInfo renderpassInfo = {};
	renderpassInfo.flags = vk::RenderPassCreateFlags();
	renderpassInfo.attachmentCount = 1;
	renderpassInfo.pAttachments = &colorAttachment;
	renderpassInfo.subpassCount = 1;
	renderpassInfo.pSubpasses = &subpass;

	try {
		renderPass = logicalDevice.createRenderPass(renderpassInfo);
	}
	catch (vk::SystemError) {
		std::cerr << RED << "[###]" << RESET << std::endl;
		error = true;
		finishPipelineSetup();
		std::abort();
	}
	std::cout << GREEN << "[###]" << RESET << std::endl;

	//finish pipeline creation
	pipelineInfo.layout = layout;
	pipelineInfo.renderPass = renderPass;
	pipelineInfo.basePipelineHandle = nullptr;

	try {
		pipeline = logicalDevice.createGraphicsPipeline(nullptr, pipelineInfo).value;
	}
	catch (vk::SystemError err) {
		error = true;
		finishPipelineSetup();
		std::abort();
	}
	finishPipelineSetup();
}
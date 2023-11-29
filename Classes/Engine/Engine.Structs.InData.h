#include <iostream>
#include <vector>

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
struct InstanceCreateInfo {
	std::vector<const char*> validationLayers = {};
	Version VulkanVersion;
	std::string AppName;
	Version AppVersion;
};
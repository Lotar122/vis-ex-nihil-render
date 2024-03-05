#pragma once
#include <vector>
namespace nihil::nstd {
	union ComponentT {
		std::vector<float>* vf;
		std::vector<uint32_t>* vui;
		void* any;
	};

	enum class ComponentTEnum {
		vfT,
		vuiT,
		anyT
	};

	class Component {
		ComponentT componentData;
		ComponentTEnum componentT;

	public:
		Component(std::vector<uint32_t>* data) {
			componentData.vui = data;
		}
		Component(std::vector<float>* data) {
			componentData.vf = data;
		}
		Component(void* data) {
			componentData.any = data;
		}

		void* get()
		{
			switch (componentT)
			{
			case ComponentTEnum::anyT:
				return componentData.any;
				break;
			case ComponentTEnum::vfT:
				return (void*)componentData.vf;
				break;
			case ComponentTEnum::vuiT:
				return (void*)componentData.vui;
				break;
			}
		}

		void set(std::vector<uint32_t>* data)
		{
			componentData.vui = data;
		}
		void set(std::vector<float>* data)
		{
			componentData.vf = data;
		}
		void set(void* data)
		{
			componentData.any = data;
		}
	};

	class ComponentArray {
		std::vector<Component> data;
		ComponentTEnum type;

	public:

		void append(std::vector<uint32_t>* data)
		{
			this->data.push_back(Component(data));
		}
		void append(std::vector<float>* data)
		{
			this->data.push_back(Component(data));
		}
		void append(void* data)
		{
			this->data.push_back(Component(data));
		}

		void assign(std::vector<uint32_t>* data, size_t index)
		{
			this->data[index] = Component(data);
		}
		void assign(std::vector<float>* data, size_t index)
		{
			this->data[index] = Component(data);
		}
		void assign(void* data, size_t index)
		{
			this->data[index] = Component(data);
		}

		void erase(size_t index)
		{
			this->data.erase(this->data.begin() + index);
		}
	};
}
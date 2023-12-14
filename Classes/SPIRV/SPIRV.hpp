#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>

namespace nihil {
	//the super-class (static) that provides functions to work with the SPIR-V format
	class SPIRV
	{
	public:
		//loads the contents od a .spv file to a vector of char's
		static std::vector<char> LoadFile(std::string filepath);
	};
}
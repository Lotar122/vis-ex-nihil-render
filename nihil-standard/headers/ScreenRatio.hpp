#pragma once
#include <iostream>
namespace nihil {
	namespace nstd {
		/*
		* @brief a struct used to store the screen ratio eg. 16:9
		*/
		struct ScreenRatio {
			/*
			* @brief this holds the width in a width:height system
			*/
			uint8_t width;
			/*
			* @brief this holds the height in a width:height system
			*/
			uint8_t height;
		};
	}
}
#pragma once
#include <iostream>
#include "ScreenRatio.hpp"
#include "WidthHeightEnum.hpp"

namespace nihil {
	namespace nstd {
		class USC 
		{
		public:
			/*
			* @brief converts USC to NDC
			* 
			* @param USC - the value to be converted in USC
			* @param ratio - the screen ratio
			* @param xy - if its the x - Width or y - Height (in means of coordinates)
			*/
			static inline float NDC_u(float USC, ScreenRatio ratio, WidthHeightEnum xy)
			{
				if (xy == WidthHeightEnum::Width) {
					return USC / 100 / ratio.width;
				}
				if (xy == WidthHeightEnum::Height) {
					return USC / 100 / ratio.height;
				}
			}
			/*
			* @brief converts NDC to USC
			*
			* @param NDC - the value to be converted in NDC
			* @param ratio - the screen ratio
			* @param xy - if its the x - Width or y - Height (in means of coordinates)
			*/
			static inline float USC_u(float NDC, ScreenRatio ratio, WidthHeightEnum xy)
			{
				if (xy == WidthHeightEnum::Width) {
					return NDC * 100 * ratio.width;
				}
				if (xy == WidthHeightEnum::Height) {
					return NDC * 100 * ratio.height;
				}
			}
		};
	}
}
#pragma once
#include <iostream>
#include "ScreenRatio.hpp"
#include "WidthHeightEnum.hpp"

namespace nihil {
	namespace nstd {
		class USC 
		{
		public:
			static inline float NDC_u(float USC, ScreenRatio ratio, WidthHeightEnum xy)
			{
				if (xy == WidthHeightEnum::Width) {
					return USC / 100 / ratio.width;
				}
				if (xy == WidthHeightEnum::Height) {
					return USC / 100 / ratio.height;
				}
			}
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
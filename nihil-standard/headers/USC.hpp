#pragma once
#include <iostream>
#include "ScreenRatio.hpp"
#include "WidthHeightEnum.hpp"

namespace nihil {
	namespace nstd {
		class USC 
		{
		private:
			static inline int64_t countDigit(int64_t n)
			{
				if (n == 0)
					return 1;
				int count = 0;
				while (n != 0) {
					n = n / 10;
					++count;
				}
				return count;
			}
			static inline int64_t pow(int64_t x, int64_t y)
			{
				int64_t res = 1;
				for (int i = 0; i < y; i++) {
					res = res * x;
				}
				return res;
			}
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
				uint8_t denominator;
				if (ratio.width > ratio.height) {
					denominator = ratio.width;
				}
				else {
					denominator = ratio.height;
				}
				if (xy == WidthHeightEnum::Width) {
					return USC / 100 / ratio.width / pow(10, countDigit(denominator));
				}
				if (xy == WidthHeightEnum::Height) {
					return USC / 100 / ratio.height / pow(10, countDigit(denominator));
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
				uint8_t denominator;
				if (ratio.width > ratio.height) {
					denominator = ratio.width;
				}
				else {
					denominator = ratio.height;
				}
				if (xy == WidthHeightEnum::Width) {
					return NDC * 100 * ratio.width * pow(10, countDigit(denominator) - 1);
				}
				if (xy == WidthHeightEnum::Height) {
					return NDC * 100 * ratio.height * pow(10, countDigit(denominator) - 1);
				}
			}
		};
	}
}
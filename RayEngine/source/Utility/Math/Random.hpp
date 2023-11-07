#pragma once

#include <random>

namespace Utility
{
	namespace Math
	{
		inline float RandomFactor()
		{
			static std::mt19937 generator;
			std::uniform_real_distribution distribution(0.0f, 1.0f);
			return distribution(generator);
		}

		inline float RandomFloat(const float aMin, const float aMax)
		{
			static std::mt19937 generator;
			std::uniform_real_distribution distribution(aMin, aMax);
			return distribution(generator);
		}
	}
}

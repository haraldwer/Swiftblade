#pragma once

#include <random>

namespace Utility
{
	namespace Math
	{
		inline float RandomFactor()
		{
			static std::default_random_engine generator;
			std::uniform_real_distribution distribution(0.0f, 1.0f);
			return distribution(generator);
		}

		inline float RandomFloat(const float InMin, const float InMax)
		{
			static std::default_random_engine generator;
			std::uniform_real_distribution distribution(InMin, InMax);
			return distribution(generator);
		}

		class Random
		{
		public: 
			Random() = default;
			Random(const uint32 InSeed)
			{
				seed.seed(InSeed);
			}

			template <class T>
			T Range(T InMin, T InMax);

			template <class T>
			T Factor()
			{
				return Range<T>(0, 1); 
			}
			
		private: 
			std::default_random_engine seed = {}; 
		};

		template <class T>
		T Random::Range(const T InMin, const T InMax)
		{
			std::uniform_real_distribution<double> distribution(InMin, InMax);
			return static_cast<T>(distribution(seed)); 
		}

		template <>
		inline int Random::Range(const int InMin, const int InMax)
		{
			std::uniform_int_distribution distribution(InMin, InMax);
			return distribution(seed); 
		}
	}
}

#pragma once
#include <chrono>

namespace Utility
{
	namespace Math
	{
		class Timer
		{
		public:
			
			Timer()
			{
				start = std::chrono::high_resolution_clock::now();
				previous = start;
			}
			
			void Update()
			{
				const std::chrono::steady_clock::time_point newTime = std::chrono::high_resolution_clock::now();
				const std::chrono::duration<double> difference = newTime - previous;
				delta = difference.count();
				previous = newTime;
			}
			
			double GetDeltaTime() const
			{
				return delta;
			}
			
			double GetTotalTime() const
			{
				const std::chrono::steady_clock::time_point newTime = std::chrono::high_resolution_clock::now();
				const std::chrono::duration<double> difference = newTime - start;
				return difference.count();
			}

		private:
			
			double delta = 1.0;
			std::chrono::steady_clock::time_point previous;
			std::chrono::steady_clock::time_point start;
		};
	}
}

#pragma once

namespace Utility
{
	namespace Math
	{
		inline float DegreesToRadians(float aDegree)
		{
			return aDegree * (PI_FLOAT / 180.0f);
		}

		inline float RadiansToDegrees(float aRadians)
		{
			return aRadians * (180.0f / PI_FLOAT);
		}
	}
}

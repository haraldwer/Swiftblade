#pragma once

namespace Utility
{
	namespace Math
	{
		static float DegreesToRadians(float aDegree)
		{
			return aDegree * (PI_FLOAT / 180.0f);
		}

		static float RadiansToDegrees(float aRadians)
		{
			return aRadians * (180.0f / PI_FLOAT);
		}
	}
}

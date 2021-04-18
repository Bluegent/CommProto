#ifndef CP_MATH_H
#define CP_MATH_H
#include <math.h>

namespace commproto
{
	namespace math
	{
		inline bool floatEq(float a, float b, float epsilon = 0.0001f)
		{
			return abs(a - b) < epsilon;
		}

		inline float getNearest(float left, float right, float value, float step)
		{
			float compute = left + value * (right - left);
			if (floatEq(step, 0.f))
			{
				return compute;
			}
			if(right - compute < step )
			{
				return right;
			}

			float div = compute / step;
			compute = floor(div) * step;
			return compute;
		}
	}
}


#endif//CP_MATH_H
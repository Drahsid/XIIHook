#include <math.h>

#pragma once

namespace Interp 
{
	enum easingTypes 
	{
		Linear,
		OnePointFive,
		Squared,
		Cubic,
		Quad,
		Quint,
		Sine
	};

	int easingTypes = 7;
	char const* easingTypeNames[] = { "Linear", "OnePointFive","Square", "Cubic", "Quad", "Quint", "Sine" };
	double PI = 3.1415926535897932;
	double TAU = 2 * PI;
	double hPI = PI / 2;

	class Interp 
	{
	public:
		float time0 = 0; // Time on first tick
		float time1 = 0; // Time at 1
		float smoothTime = 0; // Time to reach target
		float position0 = 0; // Initial position
		float position = 0; // Current position
		float target = 0; // Current target
		int type = easingTypes::Linear;
	public:
		void setType(int Type) 
		{
			type = Type;
		}
		void setType(const char* Type) 
		{
			for (int i = 0; i < easingTypes; i++) 
			{
				if (easingTypeNames[i] == Type) type = i;
			}
		}

		float interp(float&nTime)
		{
			time1 = time0 + smoothTime;
			float d = ((nTime - time0) / (time1 - time0));
			float oPf = pow(d, 1.5);
			float cS = d * d;
			float cD = cS * d;
			float cQu = cD * d;
			float cQi = cQu * d;
			float cSi = sin(d * hPI);
			switch (type) 
			{
			case easingTypes::Linear:
				position = position0 + ((target - position0) * d);
				return position;
				break;
			case easingTypes::OnePointFive:
				position = position0 + ((target - position0) * oPf);
				return position;
				break;
			case easingTypes::Squared:
				position = position0 + ((target - position0) * cS);
				return position;
				break;
			case easingTypes::Cubic:
				position = position0 + ((target - position0) * cD);
				return position;
				break;
			case easingTypes::Quad:
				position = position0 + ((target - position0) * cQu);
				return position;
				break;
			case easingTypes::Quint:
				position = position0 + ((target - position0) * cQi);
				return position;
				break;
			case easingTypes::Sine:
				position = position0 + ((target - position) * cSi);
				return position;
				break;
			}
		}

		Interp() {}
	};

}


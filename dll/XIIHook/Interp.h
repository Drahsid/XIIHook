#ifndef INTERP_H 
#define INTERP_H

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
		Sine,
		SinSq,
		SinInvSq,
		PerlinFast,
		Perlin
	};

	int easingTypes = 11;
	char const* easingTypeNames[] = { "Linear", "OnePointFive", "Square", "Cubic", "Quad", "Quint", "Sine", "SinSq", "SinInvSq", "PerlinFast", "Perlin" };
	double PI = 3.1415926535897932;
	double TAU = 2 * PI;
	float hPI = PI / 2;

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
			float cS = d * d;
			float cD = cS * d;
			float cQu = cD * d;		
			float oPf, cQi, cSi, cSS, cSInv, dFPerlin, dPerlin;

			switch (type) 
			{
			case easingTypes::Linear:
				position = position0 + ((target - position0) * d);
				return position;
				break;
			case easingTypes::OnePointFive:
				oPf = pow(d, 1.5);
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
				cQi = cQu * d;
				position = position0 + ((target - position0) * cQi);
				return position;
				break;
			case easingTypes::Sine:
				cSi = sin(d * hPI);
				position = position0 + ((target - position) * cSi);
				return position;
				break;
			case easingTypes::SinSq:
				cSS = sin(d * d * hPI);
				position = position0 + ((target - position0) * cSS);
				return position;
				break;
			case easingTypes::SinInvSq:
				cSInv = sin(pow(d, 0.5) * hPI);
				position = position0 + ((target - position0) * cSInv);
				return position;
				break;
			case easingTypes::PerlinFast:
				dFPerlin = 3 * cS - 2 * cD;
				position = position0 + ((target - position0) * dFPerlin);
				return position;
				break;
			case easingTypes::Perlin:
				dPerlin =
					   70 * d*d*d*d*d*d*d*d*d	// 70x^9
					- 315 * d*d*d*d*d*d*d*d		//315x^8
					+ 540 * d*d*d*d*d*d*d		//540x^7
					- 420 * d*d*d*d*d*d			//420x^6
					+ 126 * d*d*d*d*d;			//126x^5
				position = position0 + ((target - position0) * dPerlin);
				return position;
				break;
			}
			return 1;
		}

		Interp() {}
	};

}

#endif




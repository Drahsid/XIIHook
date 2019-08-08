#pragma once

#ifndef QUAT_H
#define QUAT_H


#include <inttypes.h>
#include <string>
#include <math.h>
#include <cmath>
#include "Vector3.h"


template <class T>
struct QuaternionBase {
	union {
		T data[4];
		struct {
			T x;
			T y;
			T z;
			T w;
		};
	};

	QuaternionBase() : x(0), y(0), z(0), w(0) {}
	QuaternionBase(T _x, T _y, T _z, T _w) : x(_x), y(_y), z(_z) {}
	QuaternionBase(const QuaternionBase&rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {}

	QuaternionBase&operator=(const QuaternionBase&rhs) { x = rhs.x; y = rhs.y; z = rhs.z; w = rhs.w; return *this; }

	bool operator==(const QuaternionBase&rhs) { return x == rhs.x && y == rhs.y && z == rhs.z && w == rhs.w; }
	bool operator!=(const QuaternionBase&rhs) { return x != rhs.x || y != rhs.y || z != rhs.z || w != rhs.w; }

	T&operator []	(const uint32_t i) { return *((T*)this[i]); }

	friend std::ostream& operator<<(std::ostream&lhs, const QuaternionBase&rhs) {
		return lhs << "( " << rhs.x << ", " << rhs.y << ", " << rhs.z << ", " << rhs.w << " )";
	}

	inline std::string toString() {
		std::ostringstream ss;
		ss << *this;
		return ss.str();
	}

	inline const char* toCharString() {
		std::ostringstream ss;
		ss << *this;
		return ss.str().c_str();
	}

	inline QuaternionBase toQuaternion(T& Pitch, T& Yaw, T& Roll) {
		T cp = cos(Pitch * 0.5);
		T sp = sin(Pitch * 0.5);
		T cy = cos(Yaw * 0.5);
		T sy = sin(Yaw * 0.5);
		T cr = cos(Roll * 0.5);
		T sr = sin(Roll * 0.5);

		QuaternionBase quat;
		quat.w = cy * cp * cr + sy * sp * sr;
		quat.x = cy * cp * sr - sy * sp * cr;
		quat.y = sy * cp * sr + cy * sp * cr;
		quat.z = sy * cp * cr - cy * sp * sr;

		return quat;
	}

	inline QuaternionBase toQuaternion(Vector3Base<T>& EulerAngles) {
		T cp = cos(EulerAngles.x * 0.5);
		T sp = sin(EulerAngles.x * 0.5);
		T cy = cos(EulerAngles.y * 0.5);
		T sy = sin(EulerAngles.y * 0.5);
		T cr = cos(EulerAngles.z * 0.5);
		T sr = sin(EulerAngles.z * 0.5);

		QuaternionBase quat;
		quat.w = cy * cp * cr + sy * sp * sr;
		quat.x = cy * cp * sr - sy * sp * cr;
		quat.y = sy * cp * sr + cy * sp * cr;
		quat.z = sy * cp * cr - cy * sp * sr;

		return quat;
	}

	Vector3Base<T> toEulerAngles()
	{
		Vector3Base<T> result;

		T srcp = +2.0 * (w * x + y * z);
		T crcp = +1.0 - 2.0 * (x * x + y * y);
		result.z = atan2(srcp, crcp);

		T sp = +2.0 * (w * y - z * x);
		if (fabs(sp) >= 1)
			result.x = copysign(M_PI / 2, sp);
		else
			result.x = asin(sp);

		T sycp = +2.0 * (w * z + x * y);
		T cycp = +1.0 - 2.0 * (y * y + z * z);
		result.y = atan2(sycp, cycp);

		return result;
	}

};

#endif



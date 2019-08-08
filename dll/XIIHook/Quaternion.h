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

	QuaternionBase operator*(const QuaternionBase&rhs) {
		return QuaternionBase<T>(
			w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y,
			w * rhs.y - x * rhs.z + y * rhs.w + z * rhs.x,
			w * rhs.z + x * rhs.y - y * rhs.x + z * rhs.w,
			w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z
			);
	}

	QuaternionBase&operator*=(const QuaternionBase&rhs) {
		x = w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y;
		y = w * rhs.y - x * rhs.z + y * rhs.w + z * rhs.x;
		z = w * rhs.z + x * rhs.y - y * rhs.x + z * rhs.w;
		w = w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z;

		return *this;
	}

	QuaternionBase operator/(const T&rhs) {
		return QuaternionBase<T>(x / rhs, y / rhs, z / rhs, w / rhs);
	}

	QuaternionBase&operator/=(const QuaternionBase&rhs) {
		this->x = w * rhs.x + x * rhs.w + y * rhs.z - z * rhs.y;
		this->y = w * rhs.y - x * rhs.z + y * rhs.w + z * rhs.x;
		this->z = w * rhs.z + x * rhs.y - y * rhs.x + z * rhs.w;
		this->w = w * rhs.w - x * rhs.x - y * rhs.y - z * rhs.z;

		return *this;
	}

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

	inline const T Magnitude() {
		return pow(w * w + x * x + y * y + z * z, 0.5);
	}

	inline const QuaternionBase Normalized() {
			return *this / Magnitude();
	}

	inline QuaternionBase toQuaternion(T& Pitch, T& Yaw, T& Roll) {
		T cp = cos(Pitch * 0.5);
		T sp = sin(Pitch * 0.5);
		T cy = cos(Yaw * 0.5);
		T sy = sin(Yaw * 0.5);
		T cr = cos(Roll * 0.5);
		T sr = sin(Roll * 0.5);

		QuaternionBase result;
		result.w = cy * cp * cr + sy * sp * sr;
		result.x = cy * cp * sr - sy * sp * cr;
		result.y = sy * cp * sr + cy * sp * cr;
		result.z = sy * cp * cr - cy * sp * sr;

		return result;
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
		T sx = x * x;
		T sy = y * y;
		T sz = z * z;
		T sw = w * w;

		return Vector3Base<T>(
			asin(2 * (w * x - y * z)),
			atan2(2 * w * y + 2 * z * x,
				1 - 2 * (x * x + y * y)),
			atan2(2 * w * z + 2 * x * y,
				1 - 2 * (z * z + x * x))
			);
	}

	inline void ToVolatile(QuaternionBase&lhs, volatile QuaternionBase&rhs) {
		rhs.x = lhs.x;
		rhs.y = lhs.y;
		rhs.z = lhs.z;
		rhs.w = lhs.w;
	}

	inline void FromVolatile(volatile QuaternionBase&lhs, QuaternionBase&rhs) {
		rhs.x = lhs.x;
		rhs.y = lhs.y;
		rhs.z = lhs.z;
		rhs.w = lhs.w;
	}

	inline void ToVolatile(QuaternionBase&lhs, volatile QuaternionBase *rhs) {
		rhs->x = lhs.x;
		rhs->y = lhs.y;
		rhs->z = lhs.z;
		rhs->w = lhs.w;
	}

	inline void FromVolatile(volatile QuaternionBase *lhs, QuaternionBase&rhs) {
		rhs.x = lhs->x;
		rhs.y = lhs->y;
		rhs.z = lhs->z;
		rhs.w = lhs->w;
	}

};

typedef QuaternionBase<float> Quaternion;

#endif



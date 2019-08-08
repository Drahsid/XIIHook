#pragma once

#ifndef VECTOR2H
#define VECTOR2H

#include <inttypes.h>
#include "pch.h"

template <class T>
struct Vector3Base
{
	union {
		T data[3];
		struct {
			T x;
			T y;
			T z;
		};
	};

	Vector3Base() : x(0), y(0), z(0) {}
	Vector3Base(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
	Vector3Base(const Vector3Base&rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {}


	Vector3Base operator +	(const Vector3Base	&rhs) { return Vector3Base<T>(x + rhs.x, y + rhs.y, z + rhs.z); }
	Vector3Base operator -	(const Vector3Base	&rhs) { return Vector3Base<T>(x - rhs.x, y - rhs.y, z - rhs.z); }
	Vector3Base&operator += (const Vector3Base	&rhs) { x += rhs.x; y += rhs.y; z += rhs.z;				return *this; }
	Vector3Base&operator -= (const Vector3Base	&rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z;				return *this; }
	Vector3Base operator *	(const Vector3Base	&rhs) { return Vector3Base<T>(x * rhs.x, y * rhs.y, z * rhs.z); }
	Vector3Base operator /	(const Vector3Base	&rhs) { return Vector3Base<T>(x / rhs.x, y / rhs.y, z / rhs.z); }
	Vector3Base&operator *= (const Vector3Base	&rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z;				return *this; }
	Vector3Base&operator /= (const Vector3Base	&rhs) { x /= rhs.x; y /= rhs.y; z /= rhs.z;				return *this; }
	Vector3Base operator *	(const T			&rhs) { return Vector3Base<T>(x * rhs, y * rhs, z * rhs); }
	Vector3Base operator /	(const T			&rhs) { return Vector3Base<T>(x / rhs, y / rhs, z / rhs); }
	Vector3Base&operator *= (const T			&rhs) { x *= rhs; y *= rhs; z *= rhs;					return *this; }
	Vector3Base&operator /= (const T			&rhs) { x /= rhs; y /= rhs;	z /= rhs;					return *this; }

	Vector3Base operator-() { return Vector3Base<T>(-x, -y, -z); }

	bool operator==(const Vector3Base&rhs) { return x == rhs.x && y == rhs.y && z == rhs.z; }
	bool operator!=(const Vector3Base&rhs) { return !(x == rhs.x && y == rhs.y && z == rhs.z); }

	T&operator []	(uint32_t i) { return *((T*)this + i); }

	friend std::ostream& operator<<(std::ostream&lhs, const Vector3Base&rhs) {
		return lhs << "( " << rhs.x << ", " << rhs.y << ", " << rhs.z << " )";
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
		return (T)pow(x * x + y * y + z * z, 0.5);
	}

	inline Vector3Base Inverse() {
		return -(*this);
	}

	inline T Dot(Vector3Base&rhs) {
		return (x * rhs.x) + (y * rhs.y) + (z * rhs.z);
	}

	inline Vector3Base Cross(const Vector3Base&rhs) {
		return Vector3Base<T>(
			y * rhs.z - rhs.y * z,
			z * rhs.x - rhs.z * x,
			x * rhs.y - rhs.x * y);
	}

	inline Vector3Base Normalized()
	{
		if (Magnitude() == 0) return Vector3Base<T>();
		return (*this) /= Magnitude();
	}

	inline bool isNaN() {
		bool notNaN = true;
		notNaN = x == x && y == y && z == z;
		return !notNaN;
	}

	inline void ToVolatile(Vector3Base&lhs, volatile Vector3Base&rhs) {
		rhs.x = lhs.x;
		rhs.y = lhs.y;
		rhs.z = lhs.z;
	}

	inline void FromVolatile(volatile Vector3Base&lhs, Vector3Base&rhs) {
		rhs.x = lhs.x;
		rhs.y = lhs.y;
		rhs.z = lhs.z;
	}

	inline void ToVolatile(Vector3Base&lhs, volatile Vector3Base *rhs) {
		rhs->x = lhs.x;
		rhs->y = lhs.y;
		rhs->z = lhs.z;
	}

	inline void FromVolatile(volatile Vector3Base *lhs, Vector3Base&rhs) {
		rhs.x = lhs->x;
		rhs.y = lhs->y;
		rhs.z = lhs->z;
	}
};



typedef Vector3Base<float>		Vector3f;
typedef Vector3Base<double>		Vector3;
typedef Vector3Base<int>		Vector3i;

#endif




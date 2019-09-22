#pragma once

#ifndef VECTOR3_H
#define VECTOR3_H

template <class T>
struct Vector3Base
{
	union {
		struct {
			T x;
			T y;
			T z;
		};
	};

	__forceinline Vector3Base() : x(0), y(0), z(0) {}
	__forceinline Vector3Base(T _x, T _y, T _z) : x(_x), y(_y), z(_z) {}
	__forceinline Vector3Base(const Vector3Base& rhs) : x(rhs.x), y(rhs.y), z(rhs.z) {}


	__forceinline Vector3Base operator +  (const Vector3Base& rhs) { return Vector3Base<T>(x + rhs.x, y + rhs.y, z + rhs.z); }
	__forceinline Vector3Base operator -  (const Vector3Base& rhs) { return Vector3Base<T>(x - rhs.x, y - rhs.y, z - rhs.z); }
	__forceinline Vector3Base operator *  (const Vector3Base& rhs) { return Vector3Base<T>(x * rhs.x, y * rhs.y, z * rhs.z); }
	__forceinline Vector3Base operator /  (const Vector3Base& rhs) { return Vector3Base<T>(x / rhs.x, y / rhs.y, z / rhs.z); }

	__forceinline Vector3Base&operator += (const Vector3Base& rhs) { x += rhs.x; y += rhs.y; z += rhs.z;				return *this; }
	__forceinline Vector3Base&operator -= (const Vector3Base& rhs) { x -= rhs.x; y -= rhs.y; z -= rhs.z;				return *this; }
	__forceinline Vector3Base&operator *= (const Vector3Base& rhs) { x *= rhs.x; y *= rhs.y; z *= rhs.z;				return *this; }
	__forceinline Vector3Base&operator /= (const Vector3Base& rhs) { x /= rhs.x; y /= rhs.y; z /= rhs.z;				return *this; }
	
	__forceinline Vector3Base operator *  (const T& rhs) { return Vector3Base<T>(x * rhs, y * rhs, z * rhs); }
	__forceinline Vector3Base operator /  (const T&	rhs) { return Vector3Base<T>(x / rhs, y / rhs, z / rhs); }

	__forceinline Vector3Base&operator *= (const T&	rhs) { x *= rhs; y *= rhs; z *= rhs;					return *this; }
	__forceinline Vector3Base&operator /= (const T&	rhs) { x /= rhs; y /= rhs;	z /= rhs;					return *this; }

	__forceinline Vector3Base operator-() { return Vector3Base<T>(-x, -y, -z); }

	__forceinline bool operator==(const Vector3Base& rhs) { return x == rhs.x && y == rhs.y && z == rhs.z; }
	__forceinline bool operator!=(const Vector3Base& rhs) { return !(x == rhs.x && y == rhs.y && z == rhs.z); }

	__forceinline T&operator []	(uint32_t i) { return *((T*)this + i); }

	__forceinline friend std::ostream& operator<<(std::ostream& lhs, const Vector3Base& rhs) {
		return lhs << "( " << rhs.x << ", " << rhs.y << ", " << rhs.z << " )";
	}

	__forceinline std::string toString() {
		std::ostringstream ss;
		ss << *this;
		return ss.str();
	}

	__forceinline const char* toCharString() {
		std::ostringstream ss;
		ss << *this;
		return ss.str().c_str();
	}

	__forceinline const T magnitude() {
		return (T)pow(x * x + y * y + z * z, 0.5);
	}

	__forceinline Vector3Base inverse() {
		return -(*this);
	}

	__forceinline T dot(Vector3Base& rhs) {
		return (x * rhs.x) + (y * rhs.y) + (z * rhs.z);
	}

	__forceinline Vector3Base cross(const Vector3Base& rhs) {
		return Vector3Base<T>(
			y * rhs.z - rhs.y * z,
			z * rhs.x - rhs.z * x,
			x * rhs.y - rhs.x * y);
	}

	__forceinline Vector3Base normalized()
	{
		if (magnitude() == 0) return Vector3Base<T>();
		return (*this) /= magnitude();
	}

	__forceinline bool isNaN() {
		bool notNaN = true;
		notNaN = x == x && y == y && z == z;
		return !notNaN;
	}

	__forceinline void toVolatile(Vector3Base& lhs, volatile Vector3Base& rhs) {
		rhs.x = lhs.x;
		rhs.y = lhs.y;
		rhs.z = lhs.z;
	}

	__forceinline void fromVolatile(volatile Vector3Base& lhs, Vector3Base& rhs) {
		rhs.x = lhs.x;
		rhs.y = lhs.y;
		rhs.z = lhs.z;
	}

	__forceinline void toVolatile(Vector3Base& lhs, volatile Vector3Base *rhs) {
		rhs->x = lhs.x;
		rhs->y = lhs.y;
		rhs->z = lhs.z;
	}

	__forceinline void fromVolatile(volatile Vector3Base *lhs, Vector3Base& rhs) {
		rhs.x = lhs->x;
		rhs.y = lhs->y;
		rhs.z = lhs->z;
	}
};



typedef Vector3Base<float>		Vector3f;
typedef Vector3Base<double>		Vector3;
typedef Vector3Base<int>		Vector3i;
typedef Vector3Base<LONG>		Vector3l;

#endif




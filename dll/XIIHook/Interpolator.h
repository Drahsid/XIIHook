#pragma once

#ifndef INTERP_H 
#define INTERP_H

#include "pch.h"

#define deltaVector (targetPosition - initialPosition)

enum EasingTypes 
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

//TODO: Optimize with something like maps
constexpr int NUM_EASING_TYPES = 11;
const char* EASING_TYPE_NAMES[] = { "Linear", "OnePointFive", "Square", "Cubic", "Quad", "Quint", "Sine", "SinSq", "SinInvSq", "PerlinFast", "Perlin" };


class Interpolator 
{
public:
	float initialTime = 0;
	float endTime = 0;
	float smoothingTime = 0;
	float initialPosition = 0;
	float currentPosition = 0;
	float targetPosition = 0;
	int easingType = EasingTypes::Linear;

public:
	void setType(int& type) 
	{
		easingType = type;
	}
	void setType(const char* type) 
	{
		for (int i = 0; i < NUM_EASING_TYPES; i++) 
		{
			if (EASING_TYPE_NAMES[i] == type) easingType = i;
		}
	}

	float interpolate(float& newTime)
	{
		//TODO: Optimize more
		endTime = initialTime + smoothingTime;
		float deltaTime = ((newTime - initialTime) / (endTime - initialTime));
		float dtSquared = deltaTime * deltaTime;
		float dtCubed = dtSquared * deltaTime;
		float dtQuart = dtCubed * deltaTime;		
		float dtOnePointFive, dtQuint, dtSin, dtSinSquared, dtSinRoot, dtFastPerlin, dtPerlin;

		switch (easingType) 
		{
		case EasingTypes::Linear:
			currentPosition = initialPosition + (deltaVector * deltaTime);
			return currentPosition;
			break;
		case EasingTypes::OnePointFive:
			dtOnePointFive = powf(deltaTime, 1.5f);
			currentPosition = initialPosition + (deltaVector * dtOnePointFive);
			return currentPosition;
			break;
		case EasingTypes::Squared:
			currentPosition = initialPosition + (deltaVector * dtSquared);
			return currentPosition;
			break;
		case EasingTypes::Cubic:
			currentPosition = initialPosition + (deltaVector * dtCubed);
			return currentPosition;
			break;
		case EasingTypes::Quad:
			currentPosition = initialPosition + (deltaVector * dtQuart);
			return currentPosition;
			break;
		case EasingTypes::Quint:
			dtQuint = dtQuart * deltaTime;
			currentPosition = initialPosition + (deltaVector * dtQuint);
			return currentPosition;
			break;
		case EasingTypes::Sine:
			dtSin = sinf(deltaTime * HPI);
			currentPosition = initialPosition + (deltaVector * dtSin);
			return currentPosition;
			break;
		case EasingTypes::SinSq:
			dtSinSquared = sinf(dtSquared * HPI);
			currentPosition = initialPosition + (deltaVector * dtSinSquared);
			return currentPosition;
			break;
		case EasingTypes::SinInvSq:
			dtSinRoot = sinf(powf(deltaTime, 0.5f) * HPI);
			currentPosition = initialPosition + (deltaVector * dtSinRoot);
			return currentPosition;
			break;
		case EasingTypes::PerlinFast:
			dtFastPerlin = 3.f * dtSquared - 2.f * dtCubed;
			currentPosition = initialPosition + (deltaVector * dtFastPerlin);
			return currentPosition;
			break;
		case EasingTypes::Perlin:
			dtPerlin =
				70.f * dtQuart * dtQuart * deltaTime	// 70x^9
				- 315.f * dtQuart * dtQuart			//315x^8
				+ 540.f * dtQuart * dtCubed			//540x^7
				- 420.f * dtQuart * dtSquared			//420x^6
				+ 126.f * dtQuart * deltaTime;		//126x^5
			currentPosition = initialPosition + (deltaVector * dtPerlin);
			return currentPosition;
			break;
		}
		return 1;
	}

	Interpolator() {}
};

template <class T>
class CriticallyDamped {
public:
	double dampening = 1;
	
	void Step(clock_t newTime) {
		clock_t dt = newTime - lastUpdate;
		double deltaTime = ((double)dt) / CLOCKS_PER_SEC;

		T f = velocity - (currentPosition - targetPosition) * (dampening * dampening * deltaTime);
		double e = 1 + dampening * deltaTime;
		velocity = f / (e * e);
		currentPosition = currentPosition + velocity * deltaTime;

		lastUpdate = newTime;
	}

	void Step(double deltaTime) {
		T f = velocity - (currentPosition - targetPosition) * (dampening * dampening * deltaTime);
		double e = 1 + dampening * deltaTime;
		velocity = f / (e * e);
		currentPosition = currentPosition + velocity * deltaTime;

		lastUpdate = clock();
	}

	T GetPosition() {
		Step(clock());
		return currentPosition;
	}

	T GetTarget() {
		return targetPosition;
	}

	T GetVelocity() {
		Step(clock());
		return velocity;
	}

	T GetPosition(double deltaTime) {
		Step(deltaTime);
		return currentPosition;
	}

	T GetTarget(double deltaTime) {
		Step(deltaTime);
		return targetPosition;
	}

	T GetVelocity(double deltaTime) {
		Step(deltaTime);
		return velocity;
	}

	void SetPosition(T rhs) {
		currentPosition = rhs;
	}

	void SetTarget(T rhs) {
		targetPosition = rhs;
	}

	void SetVelocity(T rhs) {
		velocity = rhs;
	}

	CriticallyDamped() {}
	CriticallyDamped(double d)
	{
		dampening = d;
	}
	CriticallyDamped(double d, T t)
	{
		dampening = d;
		targetPosition = t;
	}
	CriticallyDamped(double d, T c, T t)
	{
		dampening = d;
		currentPosition = c;
		targetPosition = t;
	}

private:
	clock_t lastUpdate = 0;
	T currentPosition = T();
	T targetPosition = T();
	T velocity = T();
};

typedef CriticallyDamped<Vector3> CriticallyDampedV3;

#endif




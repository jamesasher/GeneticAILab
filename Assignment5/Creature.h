#pragma once

#include <SDL.h>
#include <vector>
#include "SDL2_gfxPrimitives.h"
#include "Vector2.h"
#include "Organism.h"
#include "WorldConstants.h"


class Creature : public Organism
{
protected:
	float viewDistance = 100.0f;
	float viewDistanceSqrd = viewDistance*viewDistance;

	float baseMaxSpeed = 50.0f;
	float baseMaxSpeedSqrd = baseMaxSpeed*baseMaxSpeed;

	float currentMaxSpeed = baseMaxSpeed;
	float currentMaxSpeedSqrd = currentMaxSpeed * currentMaxSpeed;

	Vector2 velocityVector = Vector2(0, 0);

	float deacceleration = -80.0f;

	float highestEnergy = 0.0f;
	//pixels per second
	//float speed = 0.0f;
	float fitnessScore = 0.0f;
	//pixels per second second
	float acceleration = 190.0f;

	float lifeTime = 0.0f;

	float turnRate = 125.0f*TORADIANSCALAR;
	//float maxTurnRate = 125.0f*TORADIANSCALAR;

	//float baseEnergy = 60.0f;
	//float energy = baseEnergy;

	float energyConsumptionRate = 24.0f;

public:
	Creature()
	{
		color.a = 255; color.b = 255; color.g = 255; color.a = 255;
	}

	Creature(float x, float y, float direction) : Organism(x, y, direction)
	{
		//color.a = 255; color.b = 255; color.g = 255; color.a = 255;
	}

	Creature(float x, float y, float direction, float baseEnergy, float baseMaxSpeed, float acceleration, float energyConsumptionRate) : Organism(x, y, direction, baseEnergy)
	{
		//color.a = 255; color.b = 255; color.g = 255; color.a = 255;
		SetAcceleration(acceleration);
		SetBaseMaxSpeed(baseMaxSpeed);
		SetEnergyConsumptionRate(energyConsumptionRate);
	}

	~Creature()
	{

	}

	float GetViewDistance()
	{
		return viewDistance;
	}

	void UpdateVelocity(float deltaTime, const std::vector<Entity*>* Entities)
	{
		float currentSpeedSqrd = velocityVector.lengthSqrd();
		if (currentSpeedSqrd < currentMaxSpeedSqrd)
		{
			//float currentSpeed = sqrt(currentSpeedSqrd);
			velocityVector[0] = velocityVector[0] + cos(direction) * acceleration * deltaTime;
			velocityVector[1] = velocityVector[1] - sin(direction) * acceleration * deltaTime;
		}
	}

	void UpdatePosition(float deltaTime) 
	{
		x += velocityVector[0] * deltaTime;
		y += velocityVector[1] * deltaTime;
	}

	void SetViewDistance(float distance)
	{
		viewDistance = distance;
		viewDistanceSqrd = distance*distance;
	}

	/*void SetSpeed(float newSpeed)
	{
		speed = newSpeed;
	}

	float GetSpeed()
	{
		return speed;
	}*/

	void SetAcceleration(float rate)
	{
		acceleration = rate;
	}

	float GetAcceleration()
	{
		return acceleration;
	}

	void SetVelocity(Vector2 newVelocity)
	{
		velocityVector = newVelocity;
	}

	void SetVelocity(float x, float y)
	{
		velocityVector = Vector2(x, y);
	}

	Vector2 GetVelocity()
	{
		return velocityVector;
	}
	
	//Will also currentMaxSpeed
	void SetBaseMaxSpeed(float speed)
	{
		baseMaxSpeed = speed;
		baseMaxSpeedSqrd = speed*speed;
		currentMaxSpeed = baseMaxSpeed;
		currentMaxSpeedSqrd = currentMaxSpeed * currentMaxSpeed;
	}

	float GetBaseMaxSpeed()
	{
		return baseMaxSpeed;
	}

	void SetCurrentMaxSpeed(float speed)
	{
		currentMaxSpeed = speed;
		currentMaxSpeedSqrd = speed*speed;
	}

	float GetCurrentMaxSpeed()
	{
		return currentMaxSpeed;
	}

	void AddLifeTime(float Time)
	{
		lifeTime += Time;
	}

	float GetLifeTime()
	{
		return lifeTime;
	}

	float GetFitnessScore()
	{
		return fitnessScore;
	}

	void SetFitnessScore(float score)
	{
		fitnessScore = score;
	}

	void SetEnergyConsumptionRate(float scalar)
	{
		energyConsumptionRate = scalar;
	}

	float GetEnergyCosumptionRate()
	{
		return energyConsumptionRate;
	}

	//Assumed given rads arebetween 0.0 and 2*PI
	float GetAngleMinDifference(float fromRad, float toRad)
	{
		float p = toRad - fromRad;
		float phi = abs(p);
		//float sign = p / phi;
		int div = phi / (2.0f*M_PI);
		phi = phi - ((float)div*2.0f*M_PI);

		float distance = phi > M_PI ? (2.0f*M_PI) - phi : phi;
		float sign = (p >= 0 && p <= M_PI) || (p <= -M_PI && p >= -M_PI*2.0f) ? 1.0f : -1.0f;
		distance *= sign;
		return distance;
	}

	void OnEnergyChange() override
	{

		if (energy > highestEnergy)
			highestEnergy = energy;
	}

	float getHighestEnergy()
	{
		return highestEnergy;
	}

	float GetDeacceleration()
	{
		return deacceleration;
	}

	void SetDeacceleration(float rate)
	{
		if (deacceleration > 0.0)
		{
			deacceleration = -rate;
		}
		deacceleration = rate;
	}
};

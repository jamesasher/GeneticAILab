#pragma once

#include <SDL.h>
#include <vector>
#include "SDL2_gfxPrimitives.h"
#include "Vector2.h"
#include "Creature.h"
#include "StaticFunctinos.h"

class Carnivore : public Creature
{
protected:
	float wingLengthScaled = radius * 0.9;
	
	float wingAngle = 140*TORADIANSCALAR;

	float testDirection = 0.0f;

public:
	/*Carnivore()
	{
		color.a = 255; color.b = 255; color.g = 255; color.a = 255;
	}*/

	unsigned int GetEntityType() const override
	{
		return CARNIVORE;
	}

	Carnivore(float x, float y, float direction) : Creature(x, y, direction)
	{
		//color.a = 255; color.b = 255; color.g = 255; color.a = 255;
		//SetBaseMaxSpeed(60.0f);
	}

	Carnivore(float x, float y, float direction, float baseEnergy, float baseMaxSpeed, float acceleration, float energyConsumptionRate) : Creature(x, y, direction, baseEnergy, baseMaxSpeed, acceleration, energyConsumptionRate)
	{
		//color.a = 255; color.b = 255; color.g = 255; color.a = 255;
	}

	~Carnivore()
	{

	}

	void UpdateVelocity(float deltaTime, const std::vector<Entity*>* Entities)
	{
		Organism* bestMovementTarget = NULL;
		float bestMovementTargetDistanceSqrd = FLT_MAX;
		Vector2 bestMovementTargetPosition = Vector2(FLT_MAX, FLT_MAX);
		float bestMovementTargetEnergy = 0.0f;
		
		bool slowingDown = false;
		Organism* bestFeedingTarget = NULL;
		float bestFeedingTargetDistanceSqrd = FLT_MAX;
		Vector2 bestFeedingTargetPosition = Vector2(FLT_MAX, FLT_MAX);
		float bestFeedingTargetEnergy = 0.0f;

		bool withinMovementTargetRange = false;
		bool movementTargetSpeed = 0.0f;

		//Aquire Targets
		for (int i = 0; i < Entities->size(); i++)
		{
			Entity* otherEntity = Entities->at(i);
			if (otherEntity == this)
				continue;

			if (otherEntity->GetEntityType() == OMNIVORE || otherEntity->GetEntityType() == HERBIVORE)
			{
				float distSqrd = FLT_MAX;
				Vector2 position;
				GetClosestDistance(x, y, otherEntity, position, distSqrd);

				float deltaViewDist = radius + viewDistance + otherEntity->GetRadius();

				if (distSqrd < deltaViewDist*deltaViewDist)
				{
					Organism* otherOrganism = static_cast<Organism*>(otherEntity);

					if (otherOrganism)
					{
						if (otherOrganism->GetEnergy() > bestMovementTargetEnergy)
						{
							bestMovementTarget = otherOrganism;
							bestMovementTargetDistanceSqrd = distSqrd;
							bestMovementTargetPosition = position;
							bestMovementTargetEnergy = otherOrganism->GetEnergy();
							withinMovementTargetRange = false;
						}

						float deltaDist = radius + otherEntity->GetRadius();

						if (distSqrd < deltaDist*deltaDist)
						{
							//slowingDown = true;
							if (otherOrganism->GetEnergy() > bestFeedingTargetEnergy)
							{
								bestFeedingTarget = otherOrganism;
								bestFeedingTargetDistanceSqrd = distSqrd;
								bestFeedingTargetPosition = position;
								bestFeedingTargetEnergy = otherOrganism->GetEnergy();
							}
							if (bestMovementTarget == bestFeedingTarget)
							{
								withinMovementTargetRange = true;
							}
						}
					}
				}
			}
		}

		//Move towards movement target or slow down
		if (bestMovementTarget)
		{
			Vector2 dir = Vector2(bestMovementTargetPosition[0], bestMovementTargetPosition[1]);
			dir.normalize();
			float deltaDirection = GetAngleMinDifference(direction, atan2(-dir[1], dir[0]));

			testDirection = atan2(-dir[1], dir[0]);

			if (velocityVector.lengthSqrd() != 0.0f && Vector2::dot(dir, velocityVector) <= 0.4)
			{
				slowingDown = true;
			}

			double delta = turnRate*deltaTime;
			if (deltaDirection > 0)
			{
				if (delta > deltaDirection)
				{
					direction += deltaDirection;
					//printf("Meow");
				}
				else
				{
					direction += delta;
					if (direction >= 2.0f*M_PI)
						direction -= 2.0f*M_PI;
				}
			}
			else
			{
				if (delta < deltaDirection)
				{
					direction -= deltaDirection;
				}
				else
				{
					direction -= delta;
					if (direction < 0)
						direction += 2.0f*M_PI;
				}
			}
		}

		if (bestFeedingTarget)
		{
			float bestTargetDistance = sqrt(bestFeedingTargetDistanceSqrd);
			if (bestTargetDistance <= radius + bestFeedingTarget->GetRadius())
			{
				float energyConsumed = 0.0f;
				if (energyConsumptionRate*deltaTime > bestFeedingTarget->GetEnergy())
				{
					energyConsumed = pow(2.7182818284590452353602874, -0.0005*(pow(bestFeedingTarget->GetEnergy(), 2.0f))) * deltaTime * energyConsumptionRate;
					bestFeedingTarget->TagForDeletion();
					bestFeedingTarget->SetEnergy(0);
				}
				else
				{
					energyConsumed = pow(2.7182818284590452353602874, -0.0005*(pow(energyConsumptionRate, 2.0f)))  * deltaTime * energyConsumptionRate;
					bestFeedingTarget->ChangeEnergy(-energyConsumptionRate*deltaTime);
				}
				ChangeEnergy(energyConsumed);

				//slowingDown = true;
			}
		}

		//SetCurrentMaxSpeed(baseMaxSpeed / pow(scale, 1.25));

		//if ()

		float currentAcceleration = acceleration;

		if (withinMovementTargetRange)
		{
			float otherSpeed = 0.0f;
			Creature* otherCreature = static_cast<Creature*>(bestMovementTarget);
			if (otherCreature)
			{
				otherSpeed = otherCreature->GetVelocity().length();
				float mySpeed = velocityVector.length();
				if (mySpeed <= otherSpeed)
				{
					currentAcceleration = acceleration;
				}
				else
				{
					currentAcceleration = deacceleration;
				}
			}
		}
		else if (slowingDown)
		{
			currentAcceleration = deacceleration;
		}
		//float currentSpeed = sqrt(currentSpeedSqrd);

	//float currentAcceleration = acceleration;

		SetCurrentMaxSpeed(baseMaxSpeed / pow(scale, 1.25));
		if (currentMaxSpeed > baseMaxSpeed)
		{
			SetCurrentMaxSpeed(baseMaxSpeed);
		}

		Vector2 newVel = Vector2(velocityVector[0], velocityVector[1]);

		Vector2 directionVector = Vector2(cos(direction), -sin(direction));

		newVel[0] = velocityVector[0] + directionVector[0] * currentAcceleration * deltaTime;
		newVel[1] = velocityVector[1] + directionVector.y * currentAcceleration * deltaTime;

		float currentSpeedSqrd = newVel.lengthSqrd();
		if (currentAcceleration > 0.0f || Vector2::dot(directionVector, newVel) > 0.0f)
		{
			velocityVector[0] = newVel[0];
			velocityVector[1] = newVel[1];
			if (currentSpeedSqrd > currentMaxSpeedSqrd)
			{
				velocityVector.normalize();
				velocityVector *= currentMaxSpeed;
			}
		}
		else
		{
			velocityVector[0] = 0.0f;
			velocityVector[1] = 0.0f;
		}


		//Energy by speed
		float energyDelta = velocityVector.length() * -0.02 * deltaTime;

		//Constantly Lose Energy
		energyDelta -= deltaTime;

		ChangeEnergy(energyDelta);
	}

	void OnScaleChange(float scalar) override
	{
		wingLengthScaled = radius * 0.9;
	}

	void DrawAtPosition(SDL_Renderer* renderer, float xPos, float yPos) override
	{

		float p1x = xPos + cos(direction) * radius; float p1y = yPos - sin(direction) * radius;
		float p2x = xPos + cos(direction + wingAngle) * wingLengthScaled; float p2y = yPos - sin(direction + wingAngle) * wingLengthScaled;
		float p3x = xPos + cos(direction - wingAngle) * wingLengthScaled; float p3y = yPos - sin(direction - wingAngle) * wingLengthScaled;

		Sint16 vx[3] = { (Sint16)p1x, (Sint16)p2x, (Sint16)p3x };
		Sint16 vy[3] = { (Sint16)p1y, (Sint16)p2y, (Sint16)p3y };

		polygonRGBA(renderer, vx, vy, 3, color.r, color.g, color.b, color.a);

		lineRGBA(renderer, xPos, yPos, xPos + cos(testDirection)*1.1*radius, yPos - sin(testDirection)*radius*1.1, color.r*0.7, color.g*0.7, color.b*0.7, color.a);
	}
};
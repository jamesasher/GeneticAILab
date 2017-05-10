#pragma once

#include <SDL.h>
#include <vector>
#include "SDL2_gfxPrimitives.h"
#include "Vector2.h"
#include "Creature.h"
#include "Plant.h"

class Herbivore : public Creature
{
protected:
	float testDir = 0.0f;

public:
	Herbivore()
	{
		color.a = 255; color.b = 255; color.g = 255; color.a = 255;
	}

	Herbivore(float x, float y, float direction) : Creature(x, y, direction)
	{
		//color.a = 255; color.b = 255; color.g = 255; color.a = 255;
		//SetEnergy(500.0);
	}

	Herbivore(float x, float y, float direction, float baseEnergy, float baseMaxSpeed, float acceleration, float energyConsumptionRate) : Creature(x, y, direction, baseEnergy, baseMaxSpeed, acceleration, energyConsumptionRate)
	{
		//color.a = 255; color.b = 255; color.g = 255; color.a = 255;

	}

	~Herbivore()
	{

	}

	unsigned int GetEntityType() const override
	{
		return HERBIVORE;
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

		bool fleeing = false;

		//Aquire Targets
		for (int i = 0; i < Entities->size(); i++)
		{
			Entity* otherEntity = Entities->at(i);
			if (otherEntity == this)
				continue;

			if (otherEntity->GetEntityType() == PLANT && !fleeing)
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

						if (distSqrd < bestFeedingTargetDistanceSqrd)
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
			else if (otherEntity->GetEntityType() == CARNIVORE || otherEntity->GetEntityType() == OMNIVORE)
			{
				float distSqrd = FLT_MAX;
				Vector2 position;
				GetClosestDistance(x, y, otherEntity, position, distSqrd);

				float deltaViewDist = radius + viewDistance + otherEntity->GetRadius();

				if (distSqrd < deltaViewDist*deltaViewDist)
				{
					Organism* otherOrganism = static_cast<Organism*>(otherEntity);
					withinMovementTargetRange = false;

					if (otherOrganism)
					{
						fleeing = true;
						float deltaDist = radius + otherEntity->GetRadius();
						if (otherOrganism->GetEnergy() > bestMovementTargetEnergy)
						{
							bestMovementTarget = otherOrganism;
							bestMovementTargetDistanceSqrd = distSqrd;
							bestMovementTargetPosition = position;
							bestMovementTargetEnergy = otherOrganism->GetEnergy();
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
			if (fleeing)
			{
				dir *= -1;
			}
			float deltaDirection = GetAngleMinDifference(direction, atan2(-dir[1], dir[0]));

			if (!fleeing && velocityVector.lengthSqrd() != 0.0f && Vector2::dot(dir, velocityVector) <= 0.4)
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
			Plant* otherPlant = static_cast<Plant*>(bestMovementTarget);
			if (otherPlant)
			{
				float mySpeed = velocityVector.length();
				if (mySpeed > 0.0f)
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

	void DrawAtPosition(SDL_Renderer* renderer, float xPos, float yPos) override
	{

		float sizeScaled = scale*baseSize;

		Sint16 vx[5] = { 0,0,0,0,0 };
		Sint16 vy[5] = { 0,0,0,0,0 };
		


		vx[0] = (Sint16)(xPos + cos(direction) * sizeScaled);
		vx[1] = (Sint16)(xPos + cos(direction + 1.25663706144) * sizeScaled);
		vx[2] = (Sint16)(xPos + cos(direction + 2.51327412287) * sizeScaled);
		vx[3] = (Sint16)(xPos + cos(direction + 3.76991118431) * sizeScaled);
		vx[4] = (Sint16)(xPos + cos(direction + 5.02654824574) * sizeScaled);

		vy[0] = (Sint16)(yPos - sin(direction) * sizeScaled);
		vy[1] = (Sint16)(yPos - sin(direction + 1.25663706144) * sizeScaled);
		vy[2] = (Sint16)(yPos - sin(direction + 2.51327412287) * sizeScaled);
		vy[3] = (Sint16)(yPos - sin(direction + 3.76991118431) * sizeScaled);
		vy[4] = (Sint16)(yPos - sin(direction + 5.02654824574) * sizeScaled);


		polygonRGBA(renderer, vx, vy, 5, color.r, color.g, color.b, color.a);

		lineRGBA(renderer, xPos, yPos, xPos + cos(direction)*1.1*radius, yPos - sin(direction)*1.1*radius, color.r*0.3, color.g*0.3, color.b*0.3, color.a);
	}
};
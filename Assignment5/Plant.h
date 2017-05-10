#pragma once

#include <SDL.h>
#include <vector>
#include "SDL2_gfxPrimitives.h"
#include "Vector2.h"
#include "Organism.h"
#include "WorldConstants.h"

class Plant : public Organism
{
protected:
	const float maxEnergy = 220.0f;
	float growthRate = 10.0f;
	float growthRateScale = 1.0f;
	float growthVariance = 20.0f;

public:
	Plant()
	{ 
	
	}

	Plant(float x, float y, float direction) : Organism(x, y, direction)
	{ 
		SetScale(0);
		SetColor(0, 133, 25, 255);
		//growthRate = growthRate - growthVariance + RANDOMFLOAT*growthVariance*2.0f;
		//baseEnergy = 0.0f;
		SetMinimumScale(0.0f);
		energy = 1.0f;
	}

	~Plant() 
	{ 
	
	}

	unsigned int GetEntityType() const override
	{
		return PLANT;
	}

	void setGrowthRate(float rate)
	{
		growthRate = rate;
	}
	void UpdatePosition(float deltaTime) override
	{
		if (energy < maxEnergy)
		{
			ChangeEnergy(growthRate*growthRateScale*deltaTime);
			if (energy > maxEnergy)
			{
				energy = maxEnergy;
			}
		}
	}

	void DrawAtPosition(SDL_Renderer* renderer, float xPos, float yPos) override
	{
		//circleRGBA(renderer, xPos, yPos, baseSize*scale, color.r, color.g, color.b, color.a);

		//float sizeScaled = scale*baseSize;

		Sint16 vx[6] = { 0,0,0,0,0,0 };
		Sint16 vy[6] = { 0,0,0,0,0,0 };

		vx[0] = (Sint16)(xPos + cos(direction) * radius);
		vx[1] = (Sint16)(xPos + cos(direction + 1.0471975512) * radius);
		vx[2] = (Sint16)(xPos + cos(direction + 2.09439510239) * radius);
		vx[3] = (Sint16)(xPos + cos(direction + 3.14159265359) * radius);
		vx[4] = (Sint16)(xPos + cos(direction + 4.18879020479) * radius);
		vx[5] = (Sint16)(xPos + cos(direction + 5.23598775598) * radius);

		vy[0] = (Sint16)(yPos - sin(direction) * radius);
		vy[1] = (Sint16)(yPos - sin(direction + 1.0471975512) * radius);
		vy[2] = (Sint16)(yPos - sin(direction + 2.09439510239) * radius);
		vy[3] = (Sint16)(yPos - sin(direction + 3.14159265359) * radius);
		vy[4] = (Sint16)(yPos - sin(direction + 4.18879020479) * radius);
		vy[5] = (Sint16)(yPos - sin(direction + 5.23598775598) * radius);

		polygonRGBA(renderer, vx, vy, 6, color.r, color.g, color.b, color.a);
	}

};
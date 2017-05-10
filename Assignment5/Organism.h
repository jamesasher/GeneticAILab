#pragma once
#include "Entity.h"



class Organism : public Entity
{
protected:
	float baseEnergy = 60.0f;
	float energy = baseEnergy;

	float minimumScale = 0.35;

	float topEnergy = 0.0f;
	float totalEnergy = 0.0f;
public:
	Organism()
	{

	}

	Organism(float x, float y, float direction) : Entity(x, y, direction)
	{
		//color.a = 255; color.b = 255; color.g = 255; color.a = 255;
	}

	Organism(float x, float y, float direction, float baseEnergy) : Entity(x, y, direction)
	{
		//color.a = 255; color.b = 255; color.g = 255; color.a = 255;
		SetBaseEnergy(baseEnergy);
	}

	void SetBaseEnergy(float scalar)
	{
		baseEnergy = scalar;
	}

	void ChangeEnergy(float delta)
	{
		SetEnergy(energy + delta);

		/*energy += delta;
		float scaleRatio = energy / baseEnergy;
		if (scaleRatio > 0.5)
		{
			SetScale(energy / baseEnergy);
		}
		else
		{
			SetScale(0.5);
		}*/
	}

	void SetEnergy(float scalar)
	{
		energy = scalar;
		float scaleRatio = energy / baseEnergy;

		//SetCurrentMaxSpeed(baseMaxSpeed / pow(scale, 1.25));

		if (scaleRatio > minimumScale)
		{
			SetScale(energy / baseEnergy);
		}
		else
		{
			SetScale(minimumScale);
		}

		if (energy <= 0)
		{
			TagForDeletion();
		}

		OnEnergyChange();
	}

	virtual void OnEnergyChange() { }

	float GetEnergy()
	{
		return energy;
	}

	float GetBaseEnergy()
	{
		return baseEnergy;
	}

	void SetMinimumScale(float scalar)
	{
		minimumScale = scalar;
	}

	float GetMinimumScale()
	{
		return minimumScale;
	}
};

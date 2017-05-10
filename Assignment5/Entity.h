#pragma once

#include <SDL.h>
#include <vector>
#include "SDL2_gfxPrimitives.h"
#include "Vector2.h"

#define RANDOMFLOAT (float)rand() / (RAND_MAX)
#define TORADIANSCALAR M_PI / 180.0f


class Entity
{
protected:
	//In radians
	float direction = 0.0f;

	float scale = 1.0f;

	float baseSize = 15.0f;

	float x = 0.0;
	float y = 0.0;

	bool bTaggedForDeletion = false;

	SDL_Color color;

	float radius = baseSize*scale;

public:
	Entity()
	{
		color.a = 255; color.b = 255; color.g = 255; color.a = 255;
	}

	Entity(float x, float y, float direction) : x(x), y(y), direction(direction)
	{
		color.a = 255; color.b = 255; color.g = 255; color.a = 255;
	}

	~Entity()
	{

	}

	virtual unsigned int GetEntityType() const
	{
		return 0;
	}

	void SetColor(int r = -1, int g = -1, int b = -1)
	{
		if (r >= 0)
		{
			color.r = r;
		}
		if (g >= 0)
		{
			color.g = g;
		}
		if (b >= 0)
		{
			color.b = b;
		}
	}

	void SetBaseSize(float newSize)
	{
		baseSize = newSize;
		radius = scale*baseSize;
	}

	float GetBaseSize()
	{
		return baseSize;
	}

	void SetPosition(Vector2 pos)
	{
		x = pos[0];
		y = pos[1];
	}

	void SetPosition(float xPos, float yPos)
	{
		x = xPos;
		y = yPos;
	}

	void SetColor(Uint8 r = 255, Uint8 g = 255, Uint8 b = 255, Uint8 a = 255)
	{
		color.r = r; color.g = g; color.b = b; color.a = a;
	}

	void SetColor(SDL_Color newColor)
	{
		color = newColor;
	}

	Vector2 GetPosition()
	{
		return Vector2(x, y);
	}

	float GetX()
	{
		return x;
	}

	float GetY()
	{
		return y;
	}

	void SetScale(float scalar)
	{
		scale = scalar;
		radius = scale*baseSize;
		OnScaleChange(scalar);
	}

	float GetScale()
	{
		return scale;
	}

	float GetRadius()
	{
		return radius;
	}

	virtual void UpdateVelocity(float deltaTime, const std::vector<Entity*>* Entities) {}

	virtual void UpdatePosition(float deltaTime) {}

	virtual void OnScaleChange(float scalar) {};

	void TagForDeletion()
	{
		bTaggedForDeletion = true;
	}

	bool GetTagForDeletion()
	{
		return bTaggedForDeletion;
	}

	virtual void DrawAtPosition(SDL_Renderer* renderer, float xPos, float yPos) { circleRGBA(renderer, xPos, yPos, 5, color.r, color.g, color.b, color.a); }

	float operator[](int index) const
	{
		if (index == 0)
		{
			return x;
		}
		if (index == 1)
		{
			return y;
		}
		return -1;
	}

	float & operator[](int index)
	{
		if (index == 0)
		{
			return x;
		}
		if (index == 1)
		{
			return y;
		}
		float d = -1.0;
		return d;
	}
};
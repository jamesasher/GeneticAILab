#pragma once
#include "Entity.h"

#define SCREEN_WIDTH 1200;
#define SCREEN_HEIGHT 800;

static void GetClosestDistance(float x, float y, Entity* otherEntity, Vector2& bestRelativeLocation, float& bestDistanceSqrd)
{
	///150 prey and 1 preadator = 48fps 9/9 4:34 pm
	float relLocationX = (*otherEntity)[0] - x;
	float relLocationY = (*otherEntity)[1] - y;
	float rlxSqrd = relLocationX*relLocationX;
	float rlySqrd = relLocationY*relLocationY;

	// 0,0
	bestDistanceSqrd = rlxSqrd + rlySqrd;
	float bestXX = relLocationX; float bestYY = relLocationY;

	// 1,0
	float xxP = relLocationX + SCREEN_WIDTH; //float yy = relLocationY*relLocationY; //yy is the same
	float xxPxxP = xxP*xxP;
	float distSqrd = xxPxxP + rlySqrd;
	if (distSqrd < bestDistanceSqrd)
	{
		bestXX = xxP;
		bestYY = relLocationY;
		bestDistanceSqrd = distSqrd;
	}

	// -1,0
	float xxN = relLocationX - SCREEN_WIDTH;
	float xxNxxN = xxN*xxN;
	distSqrd = xxNxxN + rlySqrd;
	if (distSqrd < bestDistanceSqrd)
	{
		bestXX = xxN;
		bestYY = relLocationY;
		bestDistanceSqrd = distSqrd;
	}

	// -1, -1
	float yyN = relLocationY - SCREEN_HEIGHT;
	float yyNyyN = yyN*yyN;
	if (yyNyyN < bestDistanceSqrd)
	{
		if (xxNxxN < distSqrd)
		{
			distSqrd = xxNxxN + yyNyyN;
			if (distSqrd < bestDistanceSqrd)
			{
				bestXX = xxN;
				bestYY = yyN;
				bestDistanceSqrd = distSqrd;
			}
		}

		// 0, -1
		//xx = relLocationX - screenWidth;
		distSqrd = rlxSqrd + yyNyyN;
		if (distSqrd < bestDistanceSqrd)
		{
			bestXX = relLocationX;
			bestYY = yyN;
			bestDistanceSqrd = distSqrd;
		}

		// 1, -1
		//xx = relLocationX + screenWidth;
		if (xxPxxP < distSqrd)
		{
			distSqrd = xxPxxP + yyNyyN;
			if (distSqrd < bestDistanceSqrd)
			{
				bestXX = xxP;
				bestYY = yyN;
				bestDistanceSqrd = distSqrd;
			}
		}
	}

	// 1, 1
	float yyP = relLocationY + SCREEN_HEIGHT;
	float yyPyyP = yyP*yyP;
	if (yyPyyP < bestDistanceSqrd)
	{
		if (xxPxxP < distSqrd)
		{
			distSqrd = xxPxxP + yyPyyP;
			if (distSqrd < bestDistanceSqrd)
			{
				bestXX = xxP;
				bestYY = yyP;
				bestDistanceSqrd = distSqrd;
			}
		}

		// 0, 1
		distSqrd = rlxSqrd + yyPyyP;
		if (distSqrd < bestDistanceSqrd)
		{
			bestXX = relLocationX;
			bestYY = yyP;
			bestDistanceSqrd = distSqrd;
		}

		// -1, 1
		//xx = relLocationX - screenWidth;
		if (xxNxxN < distSqrd)
		{
			distSqrd = xxNxxN + yyPyyP;
			if (distSqrd < bestDistanceSqrd)
			{
				bestXX = xxN;
				bestYY = yyP;
				bestDistanceSqrd = distSqrd;
			}
		}
	}

	bestRelativeLocation = Vector2(bestXX, bestYY);
}
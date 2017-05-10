#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <math.h>

#include "SDL2_rotozoom.h"

class DisplayableImage
{
private:
	int OriginX = 0;
	int OriginY = 0;

	int WidthDiv2 = 0;
	int HeightDiv2 = 0;

	float WidthScale = 1.0f;
	float HeightScale = 1.0f;

	SDL_Surface* SourceImageSurface = NULL;
	SDL_Surface* DisplayImageSurface = NULL;

public:
	DisplayableImage(const char* imagePath)
	{
		SourceImageSurface = IMG_Load(imagePath);
		if (SourceImageSurface == NULL)
		{
			printf("Unable to load image \"%s\"! SDL_image Error: %s\n", imagePath, IMG_GetError());
			throw(IMG_GetError());
		}

		DisplayImageSurface = new SDL_Surface(*SourceImageSurface);

		/*DisplayImageSurface = IMG_Load(imagePath);
		if (DisplayImageSurface == NULL)
		{
			printf("Unable to load image \"%s\"! SDL_image Error: %s\n", imagePath, IMG_GetError());
			throw(IMG_GetError());
		}*/
	}

	DisplayableImage(const SDL_Surface* imageSurface)
	{
		SourceImageSurface = new SDL_Surface(*imageSurface);
		DisplayImageSurface = new SDL_Surface(*imageSurface);
	}

	~DisplayableImage()
	{
		SDL_free(SourceImageSurface);
		SDL_free(DisplayImageSurface);
	}

	void SetOrigin(int x, int y)
	{
		OriginX = x;
		OriginY = y;
	}

	void SetScale(double widthScale, double heightScale)
	{
		DisplayImageSurface = zoomSurface(SourceImageSurface, widthScale, heightScale, SMOOTHING_OFF);
	}

	void SetScale(int width, int height)
	{
		DisplayImageSurface = zoomSurface(SourceImageSurface, (double)width / (double)SourceImageSurface->w, (double)height / (double)SourceImageSurface->h, SMOOTHING_OFF);
	}
	
	void SetOriginPercentage(float xPercentage, float yPercentage)
	{
		if (xPercentage < 0)
		{
			xPercentage = 0;
		}
		else if (xPercentage > 1)
		{
			xPercentage = 1;
		}

		if (yPercentage < 0)
		{
			yPercentage = 0;
		}
		else if (yPercentage > 1)
		{
			yPercentage = 1;
		}

		OriginX = DisplayImageSurface->w*xPercentage;
		OriginY = DisplayImageSurface->h*yPercentage;
		//std::cout << OriginX << " " << OriginY << std::endl;
	}

	void BlitToSurface(SDL_Surface* Surface, int x = 0, int y = 0)
	{
		SDL_Rect* rect = new SDL_Rect();
		rect->x = x - OriginX;
		rect->y = y - OriginY;
		rect->w = DisplayImageSurface->w;
		rect->h = DisplayImageSurface->h;
		SDL_BlitSurface(DisplayImageSurface, NULL, Surface, rect);
	}
};
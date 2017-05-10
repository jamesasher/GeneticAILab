#pragma once
#include <SDL.h>
#include <SDL_image.h>
#include <string>
#include <math.h>

//#include "Color.h"
#include "SDL2_rotozoom.h"

class Image
{

private:
	Uint32* Pixels = NULL;
	SDL_Surface* ImageSurface = NULL;

	int Width = 0;
	int Height = 0;

public:

	Image(const char* imagePath)
	{
		ImageSurface = IMG_Load(imagePath);
		if (ImageSurface == NULL)
		{
			printf("Unable to load image \"%s\"! SDL_image Error: %s\n", imagePath, IMG_GetError());
			throw(IMG_GetError());
		}

		Pixels = (Uint32*)ImageSurface->pixels;

		Width = ImageSurface->w;
		Height = ImageSurface->h;
	}

	Image(const SDL_Surface* surface)
	{
		ImageSurface = new SDL_Surface(*surface);

		Pixels = (Uint32*)ImageSurface->pixels;
	}

	~Image()
	{
		SDL_free(ImageSurface);
	}

	SDL_Color GetPixelColor(int x, int y)
	{
		/*if (SDL_MUSTLOCK(ImageSurface) == 0)
		{
			SDL_LockSurface(ImageSurface);
		}*/

		Uint32 pixel = 0;

		//http://sdl.beuc.net/sdl.wiki/Pixel_Access
		int bpp = ImageSurface->format->BytesPerPixel;
		// Here p is the address to the pixel we want to retrieve 
		Uint8 *p = (Uint8 *)ImageSurface->pixels + y * ImageSurface->pitch + x * bpp;

		switch (bpp) 
		{
			case 1:
				pixel = *p;
				break;

			case 2:
				pixel = *(Uint16 *)p;
				break;

			case 3:
				if (SDL_BYTEORDER == SDL_BIG_ENDIAN)
					pixel = (p[0] << 16 | p[1] << 8 | p[2]);
				else
					pixel = (p[0] | p[1] << 8 | p[2] << 16);
				break;

			case 4:
				pixel = *(Uint32 *)p;
				break;

			//default:
				//return 0;       // shouldn't happen, but avoids warnings
		}

		SDL_Color color = SDL_Color();

		SDL_GetRGB(pixel, ImageSurface->format, &(color.r), &(color.g), &(color.b));

		/*if (SDL_MUSTLOCK(ImageSurface) == 0)
		{
			SDL_UnlockSurface(ImageSurface);
		}*/

		return color;
	}


	void SetPixelColor(SDL_Color color, int x, int y)
	{
		//SetPixelColor(color, x + y * ImageSurface->w);

		//putpixel(ImageSurface, x, y, SDL_MapRGBA(ImageSurface->format, color.r, color.g, color.b, color.a));

		//Uint32 pixel = SDL_MapRGBA(ImageSurface->format, color.r, color.g, color.b, color.a);
		//void putpixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
		//{
			/*if (SDL_MUSTLOCK(ImageSurface) == 0)
			{
				SDL_LockSurface(ImageSurface);
			}*/

			Uint32 pixel = SDL_MapRGBA(ImageSurface->format, color.r, color.g, color.b, color.a);

			int bpp = ImageSurface->format->BytesPerPixel;
			/* Here p is the address to the pixel we want to set */
			Uint8 *p = (Uint8 *)ImageSurface->pixels + y * ImageSurface->pitch + x * bpp;

			switch (bpp) {
			case 1:
				*p = pixel;
				break;

			case 2:
				*(Uint16 *)p = pixel;
				break;

			case 3:
				if (SDL_BYTEORDER == SDL_BIG_ENDIAN) {
					p[0] = (pixel >> 16) & 0xff;
					p[1] = (pixel >> 8) & 0xff;
					p[2] = pixel & 0xff;
				}
				else {
					p[0] = pixel & 0xff;
					p[1] = (pixel >> 8) & 0xff;
					p[2] = (pixel >> 16) & 0xff;
				}
				break;

			case 4:
				*(Uint32 *)p = pixel;
				break;
			}

			/*if (SDL_MUSTLOCK(ImageSurface) == 0)
			{
				SDL_UnlockSurface(ImageSurface);
			}*/
		//}
	
	}


	int GetWidth()
	{
		return Width;
	}

	int GetHeight()
	{
		return Height;
	}


	SDL_Surface* GetImageSurface()
	{
		return ImageSurface;
	}
};
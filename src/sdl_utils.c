#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <SDL/SDL.h>

#define WIDTH 64
#define HEIGHT 32
#define SCREEN_SCALE 10

SDL_Surface* get_pixel_square(SDL_Rect position)
{
    if(position.x > SCREEN_SCALE * (WIDTH-1) || position.y >  SCREEN_SCALE * (HEIGHT-1))
        errx(1,"Pixel out of range");

    SDL_Surface *square;
    square = SDL_CreateRGBSurface(SDL_HWSURFACE, SCREEN_SCALE, SCREEN_SCALE, 32, 0, 0, 0, 0);
    return square;
}

void draw_pixel_square(SDL_Surface *screen, int x, int y)
{
    SDL_Rect position;
    position.x = x * SCREEN_SCALE;
    position.y = y * SCREEN_SCALE;

    SDL_Surface *square = get_pixel_square(position);
    SDL_FillRect(square, NULL, SDL_MapRGB(screen->format, 255, 255, 255)); 
    SDL_BlitSurface(square, NULL, screen, &position);
}
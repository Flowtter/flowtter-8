#ifndef SDL_UTILS_H
#define SDL_UTILS_H

#include <SDL/SDL.h>

SDL_Surface *get_pixel_square(SDL_Rect position);
void draw_pixel_square(SDL_Surface *screen, int x, int y);
void clear_screen(SDL_Surface *screen);

#endif

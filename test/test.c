#include <assert.h>
#include <SDL/SDL.h>
#include "sdl_utils.h"

#define WIDTH 64
#define HEIGHT 32
#define SCREEN_SCALE 10

void draw_pixel_tests() {
    SDL_Rect position;
    position.x = 0;
    position.y = 0;
    SDL_Surface *square = get_pixel_square(position);
    assert(SCREEN_SCALE == square->h);
}

int main() {
    draw_pixel_tests();
    printf("\nAll tests passed\n");
}

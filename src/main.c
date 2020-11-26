#include <stdio.h>
#include <stdlib.h>
#include <err.h>

#include "constants.h"
#include "sdl_utils.h"

int main() {
    SDL_Surface *screen;

    if (SDL_Init(SDL_INIT_VIDEO) == -1)
        errx(1, "Could not initialize SDL: %s.\n", SDL_GetError());

    screen = SDL_SetVideoMode(WIDTH * SCREEN_SCALE, HEIGHT * SCREEN_SCALE, 32,
                              SDL_HWSURFACE);
    SDL_WM_SetCaption("Flowtter-8", NULL);

    draw_pixel_square(screen, 3, 3);
    draw_pixel_square(screen, 4, 4);
    draw_pixel_square(screen, 5, 5);

    SDL_Flip(screen);

    int loop = 1;
    SDL_Event event;
    while (loop) {
        SDL_WaitEvent(&event);
        if (event.type == SDL_QUIT)
            loop = 0;
    }

    return 0;
}

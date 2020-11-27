#include <stdio.h>
#include <stdlib.h>
#include <err.h>
#include <unistd.h>

#include "constants.h"
#include "chip8.h"
#include "sdl_utils.h"

void P_GRAPHICS(Chip8 *chip) {
    for (int i = 0x000; i < WIDTH * HEIGHT; i++) {
        printf("\033[1;36m%02X \033[0m", chip->graphics[i]);
    }
    printf("\n");
}

int main() {
    SDL_Surface *screen;

    if (SDL_Init(SDL_INIT_VIDEO) == -1)
        errx(1, "Could not initialize SDL: %s.\n", SDL_GetError());

    screen = SDL_SetVideoMode(WIDTH * SCREEN_SCALE, HEIGHT * SCREEN_SCALE, 32,
                              SDL_HWSURFACE);
    SDL_WM_SetCaption("Flowtter-8", NULL);

    Chip8 chip;
    chip_initialize(&chip);
    P_GRAPHICS(&chip);
    load_game(&chip, "PONG");
    printf("\033[0m");

    int loop = 1;
    SDL_Event event;
    while (loop) {
        SDL_PollEvent (&event);
        if (event.type == SDL_QUIT)
            loop = 0;

        emulate_cycle(&chip);
        if(chip.drawFlag){
            clear_screen(screen);
            emulate_graphics(&chip, screen);
            chip.drawFlag = 0;
        }
        usleep(UPD_SEC);
    }

    return 0;
}

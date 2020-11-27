#include <stdio.h>
#include <stdlib.h>
#include <err.h>

#include "constants.h"
#include "chip8.h"
#include "sdl_utils.h"

void PRINT_MEMORY(Chip8 chip) {
    for (int i = 0; i < 4096; i++) {
        printf("%d ", chip.memory[i]);
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
    load_game(&chip, "BCTEST");
    // PRINT_MEMORY(chip);
    // return 0;
    // TODO : Clock
    int loop = 1;
    SDL_Event event;
    while (loop) {
        SDL_WaitEvent(&event);
        if (event.type == SDL_QUIT)
            loop = 0;

        emulate_cycle(&chip);

        if (1 || chip.drawFlag)
            emulate_graphics(&chip, screen);
    }

    return 0;
}

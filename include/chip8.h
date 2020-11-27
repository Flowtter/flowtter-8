#ifndef CHIP_8_H
#define CHIP_8_H

#include <SDL/SDL.h>

typedef struct Chip_8 Chip8;
struct Chip_8 {
    unsigned char memory[4096];
    unsigned short opcode;
    unsigned char v[16];
    unsigned short index;
    unsigned short pc;
    unsigned char graphics[64 * 32];
    unsigned char delay_timer;
    unsigned char sound_timer;
    unsigned short stack[16];
    unsigned short sp;
    unsigned char keys[16];
    unsigned char drawFlag;
};

void chip_initialize(Chip8 *chip);
void emulate_cycle(Chip8 *chip);
void emulate_keys(Chip8 *chip, SDLKey key, unsigned char state);
void emulate_graphics(Chip8 *chip, SDL_Surface *screen);
void load_game(Chip8 *chip, char *path);

#endif // CHIP_8_H

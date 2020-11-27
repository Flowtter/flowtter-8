#include <stdlib.h>
#include <stdio.h>
#include <err.h>

#include "constants.h"
#include "sdl_utils.h"
#include "chip8.h"

#define UNUSED(x) (void) (x)

void PRINT_MEMORY(Chip8 *chip) {
    for (int i = 0x000; i < 800; i++) {
        if (i < 80)
            printf("\033[1;34m%02X \033[0m", chip->memory[i]);
        else if (i < 0x200)
            printf("\033[1;32m%02X \033[0m", chip->memory[i]);
        else
            printf("\033[1;33m%02X \033[0m", chip->memory[i]);
    }
    printf("\n");
}

void PRINT_GRAPHICS(Chip8 *chip) {
    for (int i = 0x000; i < WIDTH * HEIGHT; i++) {
        printf("\033[1;36m%02X \033[0m", chip->graphics[i]);
    }
    printf("\n");
}

unsigned char Chip8fontset[80] = {
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

void chip_initialize(Chip8 *chip) {
    chip->pc = 0x200;
    chip->opcode = 0x200;
    chip->index = 0;
    chip->sp = 0;
    chip->delay_timer = 0;
    chip->sound_timer = 0;
    chip->drawFlag = 1;
    memset(chip->memory, 0, sizeof(chip->memory));
    memset(chip->graphics, 0, sizeof(chip->graphics));
    memset(chip->stack, 0, sizeof(chip->stack));
    memset(chip->v, 0, sizeof(chip->v));
    memset(chip->keys, 0, sizeof(chip->keys));
    // Load fontset
    for (int i = 0; i < 80; i++) {
        chip->memory[i] = Chip8fontset[i];
    }
}

void emulate_graphics(Chip8 *chip, SDL_Surface *screen) {
    // PRINT_GRAPHICS(chip);
    // chip->drawFlag = 0;
    for (size_t x = 0; x < WIDTH; x++) {
        for (size_t y = 0; y < HEIGHT; y++) {
            if (chip->graphics[y * WIDTH + x]) {
                draw_pixel_square(screen, x, y);
            }
        }
    }
    SDL_Flip(screen);
}

void load_game(Chip8 *chip, char *path) {
    FILE *f;
    f = fopen(path, "rb");
    if (!f)
        errx(1, "Could not load game.\n");

    size_t unused = fread(chip->memory + 0x200, 1, 4096 - 0x0200, f);
    PRINT_MEMORY(chip);
    UNUSED(unused);
}

void unknown(Chip8 *chip) {
    printf("\033[1;31mUnknown opcode: 0x%04X\n\033[0m", chip->opcode);
    chip->pc += 2;
}

void emulate_cycle(Chip8 *chip) {
    chip->opcode = chip->memory[chip->pc] << 8 | chip->memory[chip->pc + 1];

    // printf("  %d\n", chip->memory[chip->pc] << 8 | chip->memory[chip->pc +
    // 1]); printf("  0x%04X\n",
    //       chip->memory[chip->pc] << 8 | chip->memory[chip->pc + 1]);
    // printf("& 0x%04X\n",
    //       (chip->memory[chip->pc] << 8 | chip->memory[chip->pc + 1]) &
    //       0xF000);
    // printf("| 0x%04X\n",
    //       (chip->memory[chip->pc] << 8 | chip->memory[chip->pc + 1]) &
    //       0x000F);
    // printf("Executing %04X at %04X , I:%02X SP:%02X\n", chip->opcode,
    // chip->pc,
    //       chip->index, chip->sp);
    switch (chip->opcode & 0xF000) {
        // - 0xNNNN
        // - 0x00E0
        // - 0x00EE
        case 0x0000: {
            switch (chip->opcode & 0x000F) {
                // - 0x00E0
                case 0x0000: {
                    memset(chip->graphics, 0, sizeof(chip->graphics));
                    chip->drawFlag = 1;
                    chip->pc += 2;
                    break;
                }
                // - 0x00EE
                case 0x000E: {
                    chip->pc = chip->stack[chip->sp];
                    chip->sp--;
                    chip->pc += 2;
                    break;
                }
                default:
                    unknown(chip);
                    break;
            }
            break;
        }
        // - 0x1000
        case 0x1000: {
            chip->pc = 0x0FFF & chip->opcode;
            break;
        }
        // - 0x2000
        case 0x2000: {
            chip->sp++;
            chip->stack[chip->sp & 0xF] = chip->pc;
            chip->pc = chip->opcode & 0x0FFF;
            break;
        }
        // - 0x3XNN
        case 0x3000: {
            unsigned short x = chip->opcode & 0x0F00;
            unsigned short nn = chip->opcode & 0x00FF;
            chip->pc += chip->v[x >> 8] == nn ? 4 : 2;
            break;
        }
        // - 0x4XNN
        case 0x4000: {
            unsigned short x = chip->opcode & 0x0F00;
            unsigned short nn = chip->opcode & 0x00FF;
            chip->pc += chip->v[x >> 8] != nn ? 4 : 2;
            break;
        }
        // - 0x5XY0
        case 0x5000: {
            unsigned short x = chip->opcode & 0x0F00;
            unsigned short y = chip->opcode & 0x00F0;
            chip->pc += chip->v[x >> 8] == chip->v[y >> 4] ? 4 : 2;
            break;
        }
        // - 0x6XNN
        case 0x6000: {
            unsigned short x = chip->opcode & 0x0F00;
            unsigned short nn = chip->opcode & 0x00FF;
            chip->v[x >> 8] = nn;
            chip->pc += 2;
            break;
        }
        // - 0x7XNN
        case 0x7000: {
            unsigned short x = chip->opcode & 0x0F00;
            unsigned short nn = chip->opcode & 0x00FF;
            chip->v[x >> 8] += nn;
            chip->pc += 2;
            break;
        }
        // - 0x8XY0
        // - 0x8XY1
        // - 0x8XY2
        // - 0x8XY3
        // - 0x8XY4
        // - 0x8XY5
        // - 0x8XY6
        // - 0x8XY7
        // - 0x8XYE
        case 0x8000: {
            unsigned short x = (chip->opcode & 0x0F00) >> 8;
            unsigned short y = (chip->opcode & 0x00F0) >> 4;
            switch (chip->opcode & 0x000F) {
                // - 0x8XY0
                case 0x0000: {
                    chip->v[x] = chip->v[y];
                    break;
                }
                // - 0x8XY1
                case 0x0001: {
                    chip->v[x] |= chip->v[y];
                    break;
                }
                // - 0x8XY2
                case 0x0002: {
                    chip->v[x] &= chip->v[y];
                    break;
                }
                // - 0x8XY3
                case 0x0003: {
                    chip->v[x] ^= chip->v[y];
                    break;
                }
                // - 0x8XY4
                case 0x0004: {
                    chip->v[0xF]
                      = (int) chip->v[x] + (int) chip->v[y] < 256 ? 0 : 1;
                    chip->v[x] += chip->v[y];
                    break;
                }
                // - 0x8XY5
                case 0x0005: {
                    chip->v[0xF]
                      = (int) chip->v[y] - (int) chip->v[x] >= 0 ? 1 : 0;
                    chip->v[x] -= chip->v[y];
                    break;
                }
                // - 0x8XY6
                case 0x0006: {
                    chip->v[0xF] = x & 0x7;
                    chip->v[x] >>= 1; // DOUBT ---------------------------------
                    break;
                }
                // - 0x8XY7
                case 0x0007: {
                    chip->v[0xF] = chip->v[x] > chip->v[y] ? 1 : 0;
                    chip->v[x] = chip->v[y] - chip->v[x];
                    break;
                }
                // - 0x8XYE
                case 0x000E: {
                    chip->v[0xF] = x >> 7;
                    chip->v[x] <<= 1; // DOUBT
                    break;
                }
                default:
                    unknown(chip);
            }
            chip->pc += 2;
            break;
        }
        // - 0x9XY0
        case 0x9000: {
            unsigned short x = chip->opcode & 0x0F00;
            unsigned short y = chip->opcode & 0x00F0;
            chip->pc += chip->v[x >> 8] != chip->v[y >> 4] ? 4 : 2;
            break;
        }
        // - 0xANNN
        case 0xA000: {
            chip->index = chip->opcode & 0x0FFF;
            chip->pc += 2;
            break;
        }
        // - 0xBNNN
        case 0xB000: {
            unsigned short nnn = chip->opcode & 0x0FFF;
            chip->pc = chip->v[0x0] + nnn;
            break;
        }
        // - 0xCXNN
        case 0xC000: {
            unsigned short nn = chip->opcode & 0x00FF;
            unsigned short x = chip->opcode & 0x0F00;
            chip->v[x >> 8] = rand() & nn;
            chip->pc += 2;
            break;
        }
        // - 0xDXYN
        case 0xD000: {
            int vx = chip->v[(chip->opcode & 0x0F00) >> 8];
            int vy = chip->v[(chip->opcode & 0x00F0) >> 4];
            unsigned int height = chip->opcode & 0x000F;
            chip->v[0xF] &= 0;
            for (unsigned int y = 0; y < height; y++) {
                unsigned int pixel = chip->memory[chip->index + y];
                for (unsigned int x = 0; x < 8; x++) {
                    if (pixel & (0x80 >> x)) {
                        if (chip->graphics[x + vx + (y + vy) * 64])
                            chip->v[0xF] = 1;
                        chip->graphics[x + vx + (y + vy) * 64] ^= 1;
                    }
                }
            }
            chip->drawFlag = 1;
            chip->pc += 2;
            break;
        }
        // - 0xEX9E
        // - 0xEXA1
        case 0xE000: {
            unsigned short x = chip->opcode & 0x0F00;
            switch (chip->opcode & 0x000F) {
                // - 0xEX9E
                case 0x000E: {
                    chip->pc += chip->keys[chip->v[x >> 8]] == 1 ? 4 : 2;
                    break;
                }
                // - 0xEXA1
                case 0x0001: {
                    chip->pc += chip->keys[chip->v[x >> 8]] == 0 ? 4 : 2;
                    break;
                }
                default:
                    unknown(chip);
            }
            break;
        }
        // - 0xFX07
        // - 0xFX0A
        // - 0xFX15
        // - 0xFX18
        // - 0xFX1E
        // - 0xFX29
        // - 0xFX33
        // - 0xFX55
        // - 0xFX65
        case 0xF000: {
            unsigned short x = (chip->opcode & 0x0F00) >> 8;
            switch (chip->opcode & 0x00FF) {
                // - 0xFX07
                case 0x0007: {
                    chip->v[x] = chip->delay_timer;
                    chip->pc += 2;
                    break;
                }
                // - 0xFX0A
                case 0x000A: {
                    while (1) {
                        for (size_t i = 0; i < 0xF; i++) {
                            unsigned char key = chip->keys[i];
                            if (key != 0) {
                                chip->v[x] = i;
                                goto DOUBLE_BREAK;
                            }
                        }
                    }
                DOUBLE_BREAK:
                    chip->pc += 2;
                    break;
                }
                // - 0xFX15
                case 0x0015: {
                    chip->delay_timer = chip->v[x];
                    chip->pc += 2;
                    break;
                }
                // - 0xFX18
                case 0x0018: {
                    chip->sound_timer = chip->v[x];
                    chip->pc += 2;
                    break;
                }
                // - 0xFX1E
                case 0x001E: {
                    chip->index += chip->v[x];
                    chip->pc += 2;
                    break;
                }
                // - 0xFX29
                case 0x0029: {
                    chip->index = chip->v[x] * 0x5;
                    chip->pc += 2;
                    break;
                }
                // - 0xFX33
                case 0x0033: {
                    chip->memory[chip->index] = chip->v[x] / 100;
                    chip->memory[chip->index + 1] = (chip->v[x] / 10) % 10;
                    chip->memory[chip->index + 2] = chip->v[x] % 10;
                    chip->pc += 2;
                    break;
                }
                // - 0xFX55
                case 0x0055: {
                    for (size_t i = 0; i <= x; i++)
                        chip->memory[chip->index + i] = chip->v[i];
                    chip->pc += 2;
                    break;
                }
                // - 0xFX65
                case 0x0065: {
                    for (size_t i = 0; i <= x; i++)
                        chip->v[i] = chip->memory[chip->index + i];
                    chip->pc += 2;
                    break;
                }

                default:
                    unknown(chip);
            }
            break;
        }

        default:
            unknown(chip);
            break;
    }
    if (chip->delay_timer > 0)
        --chip->delay_timer;
    if (chip->sound_timer > 0) {
        if (chip->sound_timer == 1)
            printf("BEEP!\n");
        --chip->sound_timer;
    }
}
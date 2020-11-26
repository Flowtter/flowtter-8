#include <stdlib.h>
#include <stdio.h>

#include "constants.h"
#include "chip8.h"

void chip_initialize(Chip8 *chip) {
    chip->pc = 0x200;
    chip->opcode = 0;
    chip->index = 0;
    chip->sp = 0;

    // Load fontset
    for (int i = 0; i < 80; ++i)
        chip->memory[i] = Chip8fontset[i];
}

void unknown(short opcode) {
    printf("Unknown opcode: 0x%X\n", opcode);
}

void emulateCycle(Chip8 *chip) {
    chip->opcode = chip->memory[chip->pc] << 8 | chip->memory[chip->pc + 1];
    switch (chip->opcode & 0xF000) {
        // - 0xNNNN
        // - 0x00E0
        // - 0x00EE
        case 0x0000: {
            switch (chip->opcode & 0x000F) {
                // - 0x00E0
                case 0x0000: {
                    for (int i = 0; i < WIDTH * HEIGHT; i++)
                        chip->graphics[i] = 0;
                    chip->drawFlag = 1;
                    chip->pc += 2;
                    break;
                }
                // - 0x00EE
                case 0x000E: {
                    chip->sp--;
                    chip->pc = chip->stack[chip->sp];
                    chip->pc += 2;
                    break;
                }
                default:
                    unknown(chip->opcode);
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
            chip->stack[chip->sp] = chip->pc;
            chip->sp++;
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
            break;
        }
        // - 0x7XNN
        case 0x7000: {
            unsigned short x = chip->opcode & 0x0F00;
            unsigned short nn = chip->opcode & 0x00FF;
            chip->v[x >> 8] += nn;
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
                    chip->v[0xF] = chip->v[y] > chip->v[x] ? 1 : 0;
                    chip->v[x] += chip->v[0xF];
                    break;
                }
                // - 0x8XY5
                case 0x0005: {
                    chip->v[0xF] = chip->v[y] > chip->v[x] ? 0 : 1;
                    chip->v[x] -= chip->v[y];
                    break;
                }
                // - 0x8XY6
                case 0x0006: {
                    chip->v[0xF] = x & 0x1;
                    chip->v[x] >>= 4;
                    break;
                }
                // - 0x8XY7
                case 0x0007: {
                    chip->v[0xF] = chip->v[x] > chip->v[y] ? 0 : 1;
                    chip->v[x] = chip->v[y] - chip->v[x];
                    break;
                }
                // - 0x8XYE
                case 0x000E: {
                    chip->v[0xF] = x >> 7;
                    chip->v[x] <<= 4;
                    break;
                }
                default:
                    unknown(chip->opcode);
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
            chip->v[x >> 8] = (rand() % 0xFF) & nn;
            chip->pc += 2;
            break;
        }
        // - 0xDXYN
        case 0xD000: {
            unsigned short x = (chip->opcode & 0x0F00) >> 8;
            unsigned short y = (chip->opcode & 0x00F0) >> 4;
            unsigned short n = chip->opcode & 0x000F;

            unsigned char x_pos = chip->v[x];
            unsigned char y_pos = chip->v[y];

            unsigned short pixel;
            chip->v[0xF] = 0;
            for (int y_line = 0; y_line < n; y_line++) {
                pixel = chip->memory[chip->index + y_line];

                for (int x_line = 0; x_line < 8; x_line++) {
                    if ((pixel & (0x80 >> x_line)) != 0) {
                        if (chip->graphics[(x_pos + x_line
                                            + ((y_pos + y_line) * WIDTH))]
                            == 1)
                            chip->v[0xF] = 1;
                        chip->graphics[x_pos + x_line
                                       + ((y_pos + y_line) * WIDTH)]
                          ^= 1;
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
                    unknown(chip->opcode);
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
                    unknown(chip->opcode);
            }
            break;
        }

        default:
            unknown(chip->opcode);

            if (chip->delay_timer > 0)
                --chip->delay_timer;
            if (chip->sound_timer > 0) {
                if (chip->sound_timer == 1)
                    printf("BEEP!\n");
                --chip->sound_timer;
            }
    }
}
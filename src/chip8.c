#include <stdlib.h>
#include <stdio.h>
#include "chip8.h"


void chip_initialize(Chip8 *chip) {
    chip->pc      = 0x200;
    chip->opcode  = 0;
    chip->index   = 0;
    chip->sp      = 0;

     // Load fontset
    for(int i = 0; i < 80; ++i)
        chip->memory[i] = chip8_fontset[i];
}

void emulateCycle(Chip8 *chip)
{
    chip->opcode = chip->memory[chip->pc] << 8 | chip->memory[chip->pc + 1];
    switch(chip->opcode & 0xF000)
    {
        case 0xE000:
        {
            switch(chip->opcode & 0x00FF)
            {
              case 0x009E:
                if(chip->keys[chip->registers[(chip->opcode & 0x0F00) >> 8]] != 0)
                    chip->pc += 4;
                else
                    chip->pc += 2;
              break;
            }
            break;
        }
        case 0xD000:
        {
            unsigned char x = chip->registers[(chip->opcode & 0x0F00) >> 8];
            unsigned char y = chip->registers[(chip->opcode & 0x00F0) >> 4];
            unsigned short height = chip->opcode & 0x000F;
            unsigned short pixel;
            chip->registers[0xF] = 0;
            for (int yline = 0; yline < height; yline++)
            {
              pixel = chip->memory[chip->index + yline];
              for(int xline = 0; xline < 8; xline++)
              {
                if((pixel & (0x80 >> xline)) != 0)
                {
                  if(chip->graphics[(x + xline + ((y + yline) * 64))] == 1)
                    chip->registers[0xF] = 1;                                 
                  chip->graphics[x + xline + ((y + yline) * 64)] ^= 1;
                }
              }
            }
            chip->drawFlag = 1;
            chip->pc += 2;
            break;
        }
        case 0xA000:
        {
            chip->index = chip->opcode & 0x0FFF;
            chip->pc += 2;
            break;
        }
        case 0x2000:
        {
            chip->stack[chip->sp] = chip->pc;
            chip->sp++;
            chip->pc = chip->opcode & 0x0FFF;
            break;
        }
        default:
            printf("Unknown opcode: 0x%X\n", chip->opcode);
  }

  if(chip->delay_timer > 0)
    --chip->delay_timer;

  if(chip->sound_timer > 0)
  {
    if(chip->sound_timer == 1)
        printf("BEEP!\n");
    --chip->sound_timer;
  }
}
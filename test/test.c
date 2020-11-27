#include <assert.h>
#include <SDL/SDL.h>
#include "sdl_utils.h"
#include "chip8.h"

#define WIDTH 64
#define HEIGHT 32
#define SCREEN_SCALE 10

unsigned char Chip8fontsetDebug[80] = {
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

void draw_pixel_tests() {
    SDL_Rect position;
    position.x = 0;
    position.y = 0;
    SDL_Surface *square = get_pixel_square(position);
    assert(SCREEN_SCALE == square->h);
}

void chip8_init_test(Chip8 *chip) {
    memset(chip->memory, 0, sizeof(chip->memory));
    chip->pc = 0x200;
    chip->opcode = 0x200;
    chip->index = 0;
    chip->sp = 0;
    memset(chip->graphics, 0, sizeof(chip->graphics));
    memset(chip->stack, 0, sizeof(chip->stack));
    memset(chip->v, 0, sizeof(chip->v));
    for (int i = 0; i < 80; i++)
        chip->memory[i] = Chip8fontsetDebug[i];
}

void assert_chip8(Chip8 *test, Chip8 *result) {
    if (test->pc != result->pc)
        printf("pc issue: got %d, expected %d\n", test->pc, result->pc);

    if (test->index != result->index)
        printf("index issue: got %d, expected %d\n", test->index, result->index);

    if (test->sp != result->sp)
        printf("sp issue: got %d, expected %d\n", test->sp, result->sp);

    if (result->stack) {
        for (int i = 0; i < 16; i++) {
            if (test->stack[i] != result->stack[i])
                printf("stack %d issue: got %d, expected %d\n", i, test->stack[i],
                       result->stack[i]);
        }
    }
    if (test->v) {
        for (int i = 0; i < 16; i++) {
            if (test->v[i] != result->v[i])
                printf("v %d issue: got %d, expected %d\n", i, test->v[i], result->v[i]);
        }
    }
    if (result->graphics) {
        for (int i = 0; i < 2048; i++) {
            if (test->graphics[i] != result->graphics[i])
                printf("v %d issue: got %d, expected %d\n", i, test->graphics[i],
                       result->graphics[i]);
        }
    }
}

void test_chip8(unsigned short opcode, unsigned short pc, unsigned short index,
                unsigned short sp, unsigned short stack[16],
                unsigned char v[16], unsigned char graphics[2048]) {
    Chip8 test, result;
    chip8_init_test(&test);
    chip8_init_test(&result);

    result.pc = pc;
    result.index = index;
    result.sp = sp;
    if (stack) {
        for (int i = 0; i < 16; i++)
            result.stack[i] = stack[i];
    }
    if (v) {
        for (int i = 0; i < 16; i++)
            result.v[i] = v[i];
    }
    if (graphics) {
        for (int i = 0; i < 2048; i++)
            result.graphics[i] = graphics[i];
    }
    test.opcode = opcode;
    emulate_cycle(&test);
    printf("\033[1;34mTEST : %04X\033[0m\n", opcode);
    assert_chip8(&test, &result);
}

int main() {
    draw_pixel_tests();
    // - 0x00E0
    test_chip8(0x00E0, 514, 0, 0, NULL, NULL, NULL);
    // - 0x00EE
    
    // - 0x1NNN
    test_chip8(0x1050, 0x0050, 0, 0, NULL, NULL, NULL);
    // - 0x1NNN
    test_chip8(0x2050, 0x0050, 0, 0, NULL, NULL, NULL);
}

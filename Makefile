CC := gcc

CPPFLAGS := `pkg-config --cflags sdl` -Iinclude/
CFLAGS := -Wall -Wextra -Werror -std=c99 -O3 
LDLIBS := `pkg-config --libs sdl`

DBGCFLAGS := $(CFLAGS)

S_DIR := src
TST_DIR := test

S_FILES := $(wildcard $(S_DIR)/*.c)
S_NOT_MAIN_FILES := $(filter-out src/main.c, $(S_FILES))
TST_FILES := $(TST_DIR)/*.c

all:
	$(CC) $(S_FILES) -o chip8 $(DBGCFLAGS) $(CPPFLAGS) $(LDLIBS) 

.PHONY: all test clean
test:
	$(CC) $(TST_FILES) -o chip8 $(DBGCFLAGS) $(CPPFLAGS) $(S_NOT_MAIN_FILES) $(LDLIBS)

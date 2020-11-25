CC := gcc

CPPFLAGS := `pkg-config --cflags sdl` -MMD
CFLAGS := -Wall -Wextra -Werror -std=c99 -O3
LDLIBS := `pkg-config --libs sdl` 

DBGCFLAGS := $(CFLAGS) -g -O0 -DDEBUG

S_DIR := src
H_DIR := include

S_FILES := $(S_DIR)/*.c

all: 
	$(CC) $(DBGCFLAGS) -I $(H_DIR) $(S_FILES) $(LDLIBS)
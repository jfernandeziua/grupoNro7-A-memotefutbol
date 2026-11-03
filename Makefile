CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -g
SRC = main.c juego.c audio.c archivos.c estructuras.c dibujar.c
OBJ = $(SRC:.c=.o)

ifeq ($(OS),Windows_NT)
  RAYLIB_PATH ?= C:/raylib/raylib
  TARGET = juego.exe
  RUN = $(TARGET)
  CFLAGS += -I$(RAYLIB_PATH)/src
  LDFLAGS = -L$(RAYLIB_PATH)/src -lraylib -lopengl32 -lgdi32 -lwinmm
else
  TARGET = juego
  RUN = ./$(TARGET)
  UNAME := $(shell uname)
  ifeq ($(UNAME),Darwin)
    CFLAGS += -I/opt/homebrew/include -I/usr/local/include
    LDFLAGS = -L/opt/homebrew/lib -L/usr/local/lib -lraylib -framework IOKit -framework Cocoa -framework OpenGL
  else
    LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
  endif
endif

$(TARGET): $(OBJ)
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.c memoteFutbol.h
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	$(RUN)

clean:
	$(RM) $(OBJ) $(TARGET)

.PHONY: run clean

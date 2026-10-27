# Compila MemoteFutbol (C99 + raylib). Uso: make | make run | make clean
CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -g
SRC = main.c juego.c audio.c archivos.c estructuras.c dibujar.c
OBJ = $(SRC:.c=.o)

# raylib se instala distinto segun el sistema operativo
ifeq ($(OS),Windows_NT)
  # Windows: instalador oficial de raylib (carpeta C:/raylib).
  # Si esta en otra ruta:  make RAYLIB_PATH=D:/otra/ruta
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
    # macOS (Homebrew): Apple Silicon usa /opt/homebrew, Intel usa /usr/local
    CFLAGS += -I/opt/homebrew/include -I/usr/local/include
    LDFLAGS = -L/opt/homebrew/lib -L/usr/local/lib -lraylib -framework IOKit -framework Cocoa -framework OpenGL
  else
    LDFLAGS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
  endif
endif

# Las recetas van en la misma linea, despues del ';', para no depender de tabuladores
$(TARGET): $(OBJ) ; $(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

%.o: %.c memoteFutbol.h ; $(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET) ; $(RUN)

clean: ; $(RM) $(OBJ) $(TARGET)

.PHONY: run clean

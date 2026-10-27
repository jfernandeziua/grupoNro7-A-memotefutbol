# MemoteFutbol

Juego de memoria con temática de fútbol, hecho en C con la biblioteca Raylib.
Trabajo Integrador Grupal, Grupo Nro 7-A.

## Integrantes

| Integrante | Usuario de GitHub | Módulos |
|---|---|---|
| (nombre) | @usuario | `memoteFutbol.h`, `audio.c` |
| (nombre) | @usuario | `juego.c` |
| (nombre) | @usuario | `archivos.c`, `estructuras.c` |
| (nombre) | @usuario | `dibujar.c`, `main.c` |

## Cómo ejecutarlo

Requisitos: compilador `gcc`, `make` y Raylib.

- **Mac:** `brew install raylib`
- **Windows:** instalador oficial de https://www.raylib.com y agregar `C:\raylib\w64devkit\bin` al PATH

```bash
make run
```

Desde VS Code también funciona el botón de la extensión Code Runner (usa `make run`).

## Controles

| Pantalla | Teclas |
|---|---|
| Menú | `1` `2` `3` dificultad, `P` jugadores, `T` ranking, `ENTER` jugar, `ESC` salir |
| Partida | clic para voltear, `Z` deshacer, `R` reiniciar, `ESC` menú |
| General | `M` silenciar |

## Estructura del proyecto

| Archivo | Contenido |
|---|---|
| `memoteFutbol.h` | Constantes, structs y prototipos |
| `main.c` | Ciclo principal |
| `juego.c` | Tablero, turnos, rachas (recursividad) y deshacer |
| `estructuras.c` | Pila, lista enlazada y cola |
| `archivos.c` | Lectura y escritura del ranking (`FILE*`) |
| `audio.c` | Tonos PCM con memoria dinámica y cola de sonidos |
| `dibujar.c` | Menú, cartas animadas, ranking y confetti |
| `imagenes/` | Imágenes de las cartas |

## Temas de la materia

| Tema | Dónde se aplica |
|---|---|
| Structs y `typedef` | `Carta`, `Conf`, `Movimiento`, `Juego` |
| Arreglos y matrices | Tablero `Carta c[4][6]`, `DIMS[3][2]` |
| Punteros | Todas las funciones reciben `Juego *j` |
| Archivos | `ranking.txt` con `fopen`, `fscanf`, `fprintf`, `fclose` |
| Memoria dinámica | Buffers de audio, nodos de pila, lista y cola |
| Pila (LIFO) | Deshacer movimientos |
| Lista enlazada | Ranking ordenado |
| Cola (FIFO) | Sonidos pendientes |
| Recursividad | `bonus_recursivo()` |
| Modularización | `.h` con guardas y 6 archivos `.c` |

## Trabajo con Git

- Una rama por módulo (`feature/core`, `feature/audio`, `feature/logica`, `feature/archivos`, `feature/interfaz`).
- Nadie hace push directo a `main`: todo entra por pull request con revisión de un compañero.
- Commits con prefijos `feat:`, `fix:`, `refactor:` y `docs:`, pequeños y descriptivos.
- Las versiones entregadas están marcadas como tags (`v1.0-modular`, etc.) y publicadas en Releases.

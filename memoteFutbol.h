#ifndef MEMOTEFUTBOL_H
#define MEMOTEFUTBOL_H

#include <stddef.h>   /* NULL */
#include "raylib.h"

/* ---------- Constantes del juego ---------- */
#define W 730
#define H 540
#define CW 110              /* ancho de una carta */
#define CH 100              /* alto de una carta */
#define GAP 10              /* separacion entre cartas */
#define TOP 70              /* espacio reservado para el HUD */
#define MAX_FILAS 4
#define MAX_COLS 6
#define MAX_CONF 80         /* particulas de confetti */
#define CANT_IMG 12
#define NOMBRE_MAX 11       /* 10 letras + '\0' */
#define TOP_RANKING 5
#define ARCHIVO_RANKING "ranking.txt"

/* Tipos de sonido (indices del arreglo snd[]) */
enum { SND_FLIP, SND_OK, SND_ERROR, SND_WIN, CANT_SND };

typedef enum { PANT_MENU, PANT_JUEGO, PANT_RANKING } Pantalla;

/* ---------- Structs ---------- */

/* estado: 0 = oculta, 1 = volteada, 2 = emparejada */
typedef struct { int par, estado; float flip, shake; } Carta;

/* Particula de confetti */
typedef struct { float x, y, v; Color c; } Conf;

/* Un intento (dos cartas elegidas) con lo necesario para deshacerlo */
typedef struct { int f1, c1, f2, c2, acerto, racha, turno, pts[2]; } Movimiento;

/* Pila (LIFO) de movimientos */
typedef struct NodoPila { Movimiento mov; struct NodoPila *sig; } NodoPila;

/* Lista enlazada ordenada con el ranking (menos intentos primero) */
typedef struct NodoLista {
    char nombre[NOMBRE_MAX];
    int dif, intentos;
    struct NodoLista *sig;
} NodoLista;

/* Cola (FIFO) de sonidos pendientes */
typedef struct NodoCola { int tipo; struct NodoCola *sig; } NodoCola;
typedef struct { NodoCola *frente, *final; } Cola;

/* Estado global del juego: se pasa por puntero a todas las funciones */
typedef struct {
    Carta c[MAX_FILAS][MAX_COLS];
    Conf conf[MAX_CONF];
    Sound snd[CANT_SND];
    Texture2D img[CANT_IMG];
    NodoPila *pila;
    NodoLista *ranking;
    Cola cola;
    Pantalla pantalla;
    int filas, cols, dif, jug;
    int f1, c1, f2, c2;             /* cartas elegidas (-1 si no hay) */
    int intentos, aciertos, espera, racha, turno, pts[2];
    int salir, mudo, nuevo, guardado, largo, sonando;
    char nombre[NOMBRE_MAX];
    float t;
} Juego;

/* ---------- juego.c ---------- */
void iniciar_juego(Juego *j);
void actualizar_juego(Juego *j);
void cerrar_juego(Juego *j);
void inicializar_tablero(Juego *j);
int bonus_recursivo(int racha);
Rectangle rect_carta(const Juego *j, int f, int col);

/* ---------- audio.c ---------- */
Wave generar_tono(float freq, float dur);
void cargar_sonidos(Juego *j);
void liberar_sonidos(Juego *j);
void pedir_sonido(Juego *j, int tipo);
void procesar_sonidos(Juego *j);

/* ---------- archivos.c ---------- */
void cargar_ranking(NodoLista **head);
void guardar_ranking(const NodoLista *head);

/* ---------- estructuras.c ---------- */
int apilar(NodoPila **tope, Movimiento m);
int desapilar(NodoPila **tope, Movimiento *m);
void liberar_pila(NodoPila **tope);
int insertar_ranking(NodoLista **head, const char *nombre, int dif, int intentos);
int mejor_de(const NodoLista *head, int dif);
void liberar_lista(NodoLista **head);
int encolar(Cola *q, int tipo);
int desencolar(Cola *q);
void liberar_cola(Cola *q);

/* ---------- dibujar.c ---------- */
void cargar_imagenes(Juego *j);
void liberar_imagenes(Juego *j);
void dibujar_juego(const Juego *j);

#endif

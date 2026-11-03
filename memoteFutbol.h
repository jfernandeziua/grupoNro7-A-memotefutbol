#ifndef MEMOTEFUTBOL_H
#define MEMOTEFUTBOL_H

#include <stddef.h>
#include <stdbool.h>
#include "raylib.h"

#define W 730
#define H 540
#define CW 110
#define CH 100
#define GAP 10
#define TOP 70
#define MAX_FILAS 4
#define MAX_COLS 6
#define MAX_CONF 80
#define CANT_IMG 12
#define NOMBRE_MAX 11
#define TOP_RANKING 5
#define ARCHIVO_RANKING "ranking.txt"

enum { SND_FLIP, SND_OK, SND_ERROR, SND_WIN, CANT_SND };

typedef enum { PANT_MENU, PANT_JUEGO, PANT_RANKING } Pantalla;

typedef struct {
    int par;
    int estado;
    float flip;
    float shake;
} Carta;

typedef struct {
    float x, y, v;
    Color c;
} Conf;

typedef struct {
    int f1, c1, f2, c2;
    int acerto;
    int racha;
    int turno;
    int pts[2];
} Movimiento;

typedef struct NodoPila {
    Movimiento mov;
    struct NodoPila *sig;
} NodoPila;

typedef struct NodoLista {
    char nombre[NOMBRE_MAX];
    int dif, intentos;
    struct NodoLista *sig;
} NodoLista;

typedef struct NodoCola {
    int tipo;
    struct NodoCola *sig;
} NodoCola;

typedef struct {
    NodoCola *frente;
    NodoCola *final;
} Cola;

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
    int f1, c1, f2, c2;
    int intentos, aciertos, espera, racha, turno, pts[2];
    int salir, mudo, nuevo, guardado, largo, sonando;
    char nombre[NOMBRE_MAX];
    float t;
    bool pausa;
    Music ambiente;
} Juego;

void iniciar_juego(Juego *j);
void actualizar_juego(Juego *j);
void cerrar_juego(Juego *j);
void inicializar_tablero(Juego *j);
int bonus_recursivo(int racha);
Rectangle rect_carta(const Juego *j, int f, int col);

Wave generar_tono(float freq, float dur);
void cargar_sonidos(Juego *j);
void liberar_sonidos(Juego *j);
void pedir_sonido(Juego *j, int tipo);
void procesar_sonidos(Juego *j);

void cargar_ranking(NodoLista **head);
void guardar_ranking(const NodoLista *head);

int apilar(NodoPila **tope, Movimiento m);
int desapilar(NodoPila **tope, Movimiento *m);
void liberar_pila(NodoPila **tope);
int insertar_ranking(NodoLista **head, const char *nombre, int dif, int intentos);
int mejor_de(const NodoLista *head, int dif);
void liberar_lista(NodoLista **head);
int encolar(Cola *q, int tipo);
int desencolar(Cola *q);
void liberar_cola(Cola *q);

void cargar_imagenes(Juego *j);
void liberar_imagenes(Juego *j);
void dibujar_juego(const Juego *j);

#endif
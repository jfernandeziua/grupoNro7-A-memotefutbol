#include "memoteFutbol.h"
#include <math.h>
/* Tamanio del tablero por dificultad: {filas, columnas} */
static const int DIMS[3][2] = {{3, 4}, {4, 4}, {4, 6}};
/* ================= Tablero ================= */
/* Rectangulo de la carta (f, col), con el tablero centrado en la ventana */
Rectangle rect_carta(const Juego *j, int f, int col)
{
    float x0 = (W - (j->cols * (CW + GAP) - GAP)) / 2;
    float y0 = TOP + (H - TOP - (j->filas * (CH + GAP) - GAP)) / 2;
    return (Rectangle){x0 + col * (CW + GAP), y0 + f * (CH + GAP), CW, CH};
}
/* Arma el tablero: dos cartas por cada par y las mezcla (Fisher-Yates) */
void inicializar_tablero(Juego *j)
{
    int par[MAX_FILAS * MAX_COLS], total;
    if (j == NULL || j->dif < 0 || j->dif > 2)
        return;

    j->filas = DIMS[j->dif][0];

    j->cols = DIMS[j->dif][1];

    total = j->filas * j->cols;

    for (int i = 0; i < total; i++)
        par[i] = i / 2;

    for (int i = total - 1; i > 0; i--)
    {

        int k = GetRandomValue(0, i), aux = par[i];

        par[i] = par[k];

        par[k] = aux;
    }

    for (int i = 0; i < total; i++)

        j->c[i / j->cols][i % j->cols] = (Carta){par[i], 0, 0, 0};
}

/* Bonus por racha, calculado con recursividad.

   bonus(1) = 100, bonus(2) = 100 + 100 = 200, bonus(3) = 150 + 200 = 350... */

int bonus_recursivo(int racha)
{

    if (racha <= 1)
        return 100; /* caso base */

    return racha * 50 + bonus_recursivo(racha - 1); /* paso recursivo */
}

static int total_pares(const Juego *j) { return j->filas * j->cols / 2; }

static int partida_ganada(const Juego *j) { return j->aciertos == total_pares(j); }

/* Estamos pidiendo el nombre cuando se gano en 1 jugador y todavia no se guardo */

static int escribiendo_nombre(const Juego *j)
{

    return j->pantalla == PANT_JUEGO && partida_ganada(j) && !j->guardado;
}

static void reiniciar_partida(Juego *j)
{

    inicializar_tablero(j);

    liberar_pila(&j->pila);

    j->f1 = j->f2 = -1;

    j->intentos = j->aciertos = j->espera = j->racha = j->turno = 0;

    j->pts[0] = j->pts[1] = 0;

    j->nuevo = j->guardado = j->largo = 0;

    j->nombre[0] = '\0';

    j->t = 0;
}

/* Avanza la animacion de volteo y de temblor de cada carta */

static void animar_cartas(Juego *j)
{

    float paso = 6 * GetFrameTime();

    for (int f = 0; f < j->filas; f++)

        for (int col = 0; col < j->cols; col++)
        {

            Carta *k = &j->c[f][col];

            float meta = (k->estado != 0) ? 1.0f : 0.0f;

            k->flip += fmaxf(-paso, fminf(paso, meta - k->flip));

            if (k->shake > 0)
                k->shake--;
        }
}
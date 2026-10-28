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
/* ================= Jugadas ================= */

static void terminar_partida(Juego *j)
{

    pedir_sonido(j, SND_WIN);

    for (int k = 0; k < MAX_CONF; k++)

        j->conf[k] = (Conf){GetRandomValue(0, W), GetRandomValue(-H, 0), GetRandomValue(2, 5),

                            ColorFromHSV(GetRandomValue(0, 360), .8f, 1)};

    j->guardado = (j->jug != 1); /* el nombre solo se pide en 1 jugador */
}

static void registrar_acierto(Juego *j, Carta *a, Carta *b)
{

    a->estado = b->estado = 2;

    j->f1 = j->f2 = -1;

    j->racha++;

    j->pts[j->turno] += bonus_recursivo(j->racha);

    pedir_sonido(j, SND_OK);

    if (++j->aciertos == total_pares(j))
        terminar_partida(j);
}

static void registrar_error(Juego *j, Carta *a, Carta *b)
{

    a->shake = b->shake = 20;

    j->espera = 60; /* 1 segundo mostrando las cartas antes de taparlas */

    j->racha = 0;

    pedir_sonido(j, SND_ERROR);

    if (j->jug == 2)
        j->turno = !j->turno;
}

/* Compara las dos cartas elegidas y guarda el intento en la pila */

static void evaluar_pareja(Juego *j)
{

    Carta *a = &j->c[j->f1][j->c1], *b = &j->c[j->f2][j->c2];

    int acerto = (a->par == b->par);

    Movimiento m = {j->f1, j->c1, j->f2, j->c2, acerto, j->racha, j->turno, {j->pts[0], j->pts[1]}};

    apilar(&j->pila, m);

    if (acerto)
        registrar_acierto(j, a, b);

    else
        registrar_error(j, a, b);
}

static void voltear_carta(Juego *j, int f, int col)
{

    j->c[f][col].estado = 1;

    pedir_sonido(j, SND_FLIP);

    if (j->f1 < 0)
    { /* es la primera carta del intento */

        j->f1 = f;

        j->c1 = col;

        return;
    }

    j->f2 = f;

    j->c2 = col;

    j->intentos++;

    evaluar_pareja(j);
}

static void manejar_clic(Juego *j)
{

    Vector2 m = GetMousePosition();

    for (int f = 0; f < j->filas; f++)

        for (int col = 0; col < j->cols; col++)
        {

            if (j->c[f][col].estado != 0)
                continue;

            if (!CheckCollisionPointRec(m, rect_carta(j, f, col)))
                continue;

            voltear_carta(j, f, col);

            return;
        }
}

static void ocultar_seleccion(Juego *j)
{

    if (j->f1 < 0 || j->f2 < 0)
        return;

    j->c[j->f1][j->c1].estado = 0;

    j->c[j->f2][j->c2].estado = 0;

    j->f1 = j->f2 = -1;
}

/* Deshacer (pop de la pila): vuelve a tapar las cartas y restaura puntos,

   racha y turno. Los intentos NO se restan, asi no se puede "hacer trampa"

   con el ranking. */

static void deshacer_movimiento(Juego *j)
{

    Movimiento m;

    if (j->f1 >= 0 && j->espera == 0)
        return; /* hay una carta elegida a medias */

    if (!desapilar(&j->pila, &m))
        return;

    j->c[m.f1][m.c1].estado = 0;

    j->c[m.f2][m.c2].estado = 0;

    if (m.acerto)
        j->aciertos--;

    j->racha = m.racha;

    j->turno = m.turno;

    j->pts[0] = m.pts[0];

    j->pts[1] = m.pts[1];

    j->f1 = j->f2 = -1;

    j->espera = 0;
}
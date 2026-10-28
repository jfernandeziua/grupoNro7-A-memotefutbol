#include "memoteFutbol.h"
#include <math.h>

static const char *NOMBRES[CANT_IMG] = {"MESSI", "COPA", "PELOTA", "GUANTES",
    "SILBATO", "BOTIN", "TARJETA", "ARCO", "CAMISETA", "CANCHA", "GOL", "DT"};
static const char *ARCHIVOS_IMG[CANT_IMG] = {"messi", "copa", "pelota", "guantes",
    "silbato", "botin", "tarjeta", "arco", "camiseta", "cancha", "gol", "dt"};

/* ================= Imagenes ================= */
void cargar_imagenes(Juego *j) {
    if (j == NULL) return;
    for (int i = 0; i < CANT_IMG; i++) {
        j->img[i] = LoadTexture(TextFormat("imagenes/%s.png", ARCHIVOS_IMG[i]));
        if (j->img[i].id > 0) { /* mipmaps + filtro trilineal: se ven nitidas al achicarse */
            GenTextureMipmaps(&j->img[i]);
            SetTextureFilter(j->img[i], TEXTURE_FILTER_TRILINEAR);
        }
    }
}

void liberar_imagenes(Juego *j) {
    if (j == NULL) return;
    for (int i = 0; i < CANT_IMG; i++) UnloadTexture(j->img[i]);
}

/* ================= Utilidades ================= */
/* Escribe un texto centrado dentro de un rectangulo */
static void centrar_texto(const char *t, Rectangle r, int tam, Color col) {
    int x = (int)(r.x + (r.width - MeasureText(t, tam)) / 2);
    int y = (int)(r.y + (r.height - tam) / 2);
    DrawText(t, x, y, tam, col);
}

/* Cancha de futbol: franjas verdes, linea central y circulo */
static void dibujar_fondo(void) {
    Color claro = {25, 110, 50, 255}, oscuro = {20, 95, 42, 255};
    for (int i = 0; i < 10; i++)
        DrawRectangle(i * W / 10, 0, W / 10, H, (i % 2) ? claro : oscuro);
    DrawLine(W / 2, 0, W / 2, H, Fade(RAYWHITE, .2f));
    DrawCircleLines(W / 2, H / 2, 80, Fade(RAYWHITE, .2f));
}

/* ================= Cartas ================= */
/* Reverso de la carta: una pelota sobre fondo azul */
static void dibujar_dorso(Rectangle d, int hover) {
    int cx = (int)(d.x + d.width / 2), cy = (int)(d.y + CH / 2);
    DrawRectangleRounded(d, .15f, 6, hover ? BLUE : DARKBLUE);
    DrawEllipse(cx, cy, 28 * d.width / CW, 28, RAYWHITE);
    DrawEllipse(cx, cy, 9 * d.width / CW, 9, BLACK);
}

/* Frente de la carta: la imagen conserva su proporcion y ocupa casi toda la ficha */
static void dibujar_cara(const Juego *j, const Carta *k, Rectangle d) {
    Texture2D t;
    float s, iw, ih;
    if (k->par < 0 || k->par >= CANT_IMG) return;
    t = j->img[k->par];
    DrawRectangleRounded(d, .15f, 6, (k->estado == 2) ? GOLD : RAYWHITE);
    if (t.id == 0) { /* si falta el archivo de la imagen, se muestra el nombre */
        if (d.width > CW * .7f) centrar_texto(NOMBRES[k->par], d, 18, BLACK);
        return;
    }
    s = fminf((CW - 16.0f) / t.width, (CH - 16.0f) / t.height);
    iw = t.width * s * d.width / CW; /* el ancho se achica con el volteo */
    ih = t.height * s;
    DrawTexturePro(t, (Rectangle){0, 0, (float)t.width, (float)t.height},
                   (Rectangle){d.x + (d.width - iw) / 2, d.y + (CH - ih) / 2, iw, ih},
                   (Vector2){0, 0}, 0, WHITE);
}

/* Dibuja una carta con sombra, hover, temblor (error) y volteo animado */
static void dibujar_carta(const Juego *j, int f, int col) {
    const Carta *k = &j->c[f][col];
    Rectangle r = rect_carta(j, f, col), d;
    int hover = (k->estado == 0 && j->espera == 0 &&
                 CheckCollisionPointRec(GetMousePosition(), r));
    float w = CW * fabsf(cosf(k->flip * PI)); /* al girar, el ancho pasa por cero */
    r.y -= hover * 5;
    r.x += sinf(k->shake * 1.5f) * k->shake * .3f;
    DrawRectangleRounded((Rectangle){r.x + 4, r.y + 5, CW, CH}, .15f, 6, Fade(BLACK, .3f));
    d = (Rectangle){r.x + (CW - w) / 2, r.y, w, CH};
    if (k->flip < .5f) dibujar_dorso(d, hover);
    else dibujar_cara(j, k, d);
}

static void dibujar_tablero(const Juego *j) {
    for (int f = 0; f < j->filas; f++)
        for (int col = 0; col < j->cols; col++)
            dibujar_carta(j, f, col);
}

/* ================= Interfaz ================= */
static void dibujar_hud(const Juego *j) {
    int mejor = mejor_de(j->ranking, j->dif);
    DrawRectangle(0, 0, W, TOP - 10, Fade(BLACK, .4f));
    if (j->jug == 1)
        DrawText(TextFormat("Intentos: %d   Tiempo: %ds   Puntos: %d   Racha: x%d   Mejor: %s",
                            j->intentos, (int)j->t, j->pts[0], j->racha,
                            mejor ? TextFormat("%d", mejor) : "-"), 15, 8, 18, RAYWHITE);
    else
        DrawText(TextFormat("J1: %d   J2: %d   Turno: J%d   Racha: x%d",
                            j->pts[0], j->pts[1], j->turno + 1, j->racha), 15, 8, 18, RAYWHITE);
    DrawText("Z: deshacer   R: reiniciar   ESC: menu", 15, 36, 14, LIGHTGRAY);
}

static void dibujar_menu(const Juego *j) {
    const char *niv[] = {"1 - Facil (3x4)", "2 - Medio (4x4)", "3 - Dificil (4x6)"};
    centrar_texto("MEMOTEFUTBOL", (Rectangle){0, 70, W, 60}, 56, GOLD);
    for (int k = 0; k < 3; k++)
        centrar_texto(niv[k], (Rectangle){0, 190 + k * 40, W, 36}, 26, (k == j->dif) ? GOLD : RAYWHITE);
    centrar_texto(TextFormat("P - Jugadores: %d", j->jug), (Rectangle){0, 330, W, 36}, 26, RAYWHITE);
    centrar_texto("T: ranking     M: sonido", (Rectangle){0, 410, W, 30}, 20, LIGHTGRAY);
    centrar_texto("ENTER: jugar     ESC: salir", (Rectangle){0, 440, W, 30}, 20, LIGHTGRAY);
}

/* Muestra los mejores TOP_RANKING de la dificultad elegida (recorre la lista) */
static void dibujar_ranking(const Juego *j) {
    const char *niv[] = {"Facil", "Medio", "Dificil"};
    const NodoLista *p;
    int n = 0;
    centrar_texto(TextFormat("RANKING - %s", niv[j->dif]), (Rectangle){0, 50, W, 50}, 40, GOLD);
    for (p = j->ranking; p != NULL && n < TOP_RANKING; p = p->sig) {
        if (p->dif != j->dif) continue;
        centrar_texto(TextFormat("%d. %s - %d intentos", n + 1, p->nombre, p->intentos),
                      (Rectangle){0, 140 + n * 45, W, 40}, 26, RAYWHITE);
        n++;
    }
    if (n == 0)
        centrar_texto("Todavia no hay partidas guardadas", (Rectangle){0, 200, W, 40}, 24, LIGHTGRAY);
    centrar_texto("1/2/3: dificultad     ESC: volver", (Rectangle){0, H - 70, W, 30}, 20, LIGHTGRAY);
}

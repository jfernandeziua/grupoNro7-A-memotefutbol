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

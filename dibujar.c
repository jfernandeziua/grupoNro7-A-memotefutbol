#include "memoteFutbol.h"
#include <math.h>

static const char *NOMBRES[CANT_IMG] = {"MESSI", "COPA", "PELOTA", "GUANTES",
    "SILBATO", "BOTIN", "TARJETA", "ARCO", "CAMISETA", "CANCHA", "GOL", "DT"};
static const char *ARCHIVOS_IMG[CANT_IMG] = {"messi", "copa", "pelota", "guantes",
    "silbato", "botin", "tarjeta", "arco", "camiseta", "cancha", "gol", "dt"};

void cargar_imagenes(Juego *j) {
    if (j == NULL) return;
    for (int i = 0; i < CANT_IMG; i++) {
        j->img[i] = LoadTexture(TextFormat("imagenes/%s.png", ARCHIVOS_IMG[i]));
        if (j->img[i].id > 0) { 
            GenTextureMipmaps(&j->img[i]);
            SetTextureFilter(j->img[i], TEXTURE_FILTER_TRILINEAR);
        }
    }
}

void liberar_imagenes(Juego *j) {
    if (j == NULL) return;
    for (int i = 0; i < CANT_IMG; i++) UnloadTexture(j->img[i]);
}

static void centrar_texto(const char *t, Rectangle r, int tam, Color col) {
    int x = (int)(r.x + (r.width - MeasureText(t, tam)) / 2);
    int y = (int)(r.y + (r.height - tam) / 2);
    DrawText(t, x, y, tam, col);
}

static void dibujar_fondo(void) {
    Color claro = {25, 110, 50, 255}, oscuro = {20, 95, 42, 255};
    for (int i = 0; i < 10; i++)
        DrawRectangle(i * W / 10, 0, W / 10, H, (i % 2) ? claro : oscuro);
    DrawLine(W / 2, 0, W / 2, H, Fade(RAYWHITE, .2f));
    DrawCircleLines(W / 2, H / 2, 90, Fade(RAYWHITE, .2f));
}

static void dibujar_dorso(Rectangle d, int hover) {
    int cx = (int)(d.x + d.width / 2), cy = (int)(d.y + CH / 2);
    DrawRectangleRounded(d, .15f, 6, hover ? BLUE : DARKBLUE);
    DrawEllipse(cx, cy, 35 * d.width / CW, 35, RAYWHITE);
    DrawEllipse(cx, cy, 12 * d.width / CW, 12, BLACK);
}

static void dibujar_cara(const Juego *j, const Carta *k, Rectangle d) {
    Texture2D t;
    float s, iw, ih;
    if (k->par < 0 || k->par >= CANT_IMG) return;
    t = j->img[k->par];
    DrawRectangleRounded(d, .15f, 6, (k->estado == 2) ? GOLD : RAYWHITE);
    if (t.id == 0) { 
        if (d.width > CW * .7f) centrar_texto(NOMBRES[k->par], d, 20, BLACK);
        return;
    }
    s = fminf((CW - 20.0f) / t.width, (CH - 20.0f) / t.height);
    iw = t.width * s * d.width / CW; 
    ih = t.height * s;
    DrawTexturePro(t, (Rectangle){0, 0, (float)t.width, (float)t.height},
                   (Rectangle){d.x + (d.width - iw) / 2, d.y + (CH - ih) / 2, iw, ih},
                   (Vector2){0, 0}, 0, WHITE);
}

static void dibujar_carta(const Juego *j, int f, int col) {
    const Carta *k = &j->c[f][col];
    Rectangle r = rect_carta(j, f, col), d;
    int hover = (k->estado == 0 && j->espera == 0 && !j->pausa &&
                 CheckCollisionPointRec(posicion_mouse(), r));
    float w = CW * fabsf(cosf(k->flip * PI)); 
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

static void dibujar_hud(const Juego *j) {
    int mejor = mejor_de(j->ranking, j->dif);
    DrawRectangle(0, 0, W, TOP - 10, Fade(BLACK, .4f));
    if (j->jug == 1)
        DrawText(TextFormat("Intentos: %d   Tiempo: %ds   Puntos: %d   Racha: x%d   Mejor: %s",
                            j->intentos, (int)j->t, j->pts[0], j->racha,
                            mejor ? TextFormat("%d", mejor) : "-"), 20, 10, 20, RAYWHITE);
    else
        DrawText(TextFormat("J1: %d   J2: %d   Turno: J%d   Racha: x%d",
                            j->pts[0], j->pts[1], j->turno + 1, j->racha), 20, 10, 20, RAYWHITE);
    DrawText("Z: deshacer   R: reiniciar   ESC: menu   P: pausa   F: pantalla completa", 20, 38, 16, LIGHTGRAY);
    
    DrawRectangle(0, 0, (int)(W * (j->t / 60.0f)), 5, GREEN);
}

static void dibujar_menu(const Juego *j) {
    const char *niv[] = {"1 - Facil (3x4)", "2 - Medio (4x4)", "3 - Dificil (4x6)"};
    centrar_texto("MEMOTEFUTBOL", (Rectangle){0, 90, W, 70}, 64, GOLD);
    for (int k = 0; k < 3; k++)
        centrar_texto(niv[k], (Rectangle){0, 230 + k * 48, W, 40}, 28, (k == j->dif) ? GOLD : RAYWHITE);
    centrar_texto(TextFormat("P - Jugadores: %d", j->jug), (Rectangle){0, 410, W, 40}, 28, RAYWHITE);
    centrar_texto("T: ranking     M: sonido     F: pantalla completa", (Rectangle){0, 500, W, 30}, 22, LIGHTGRAY);
    centrar_texto("ENTER: jugar     ESC: salir", (Rectangle){0, 545, W, 30}, 22, LIGHTGRAY);
}

static void dibujar_ranking(const Juego *j) {
    const char *niv[] = {"Facil", "Medio", "Dificil"};
    const NodoLista *p;
    int n = 0;
    centrar_texto(TextFormat("RANKING - %s", niv[j->dif]), (Rectangle){0, 60, W, 60}, 44, GOLD);
    for (p = j->ranking; p != NULL && n < TOP_RANKING; p = p->sig) {
        if (p->dif != j->dif) continue;
        centrar_texto(TextFormat("%d. %s - %d intentos", n + 1, p->nombre, p->intentos),
                      (Rectangle){0, 170 + n * 55, W, 45}, 28, RAYWHITE);
        n++;
    }
    if (n == 0)
        centrar_texto("Todavia no hay partidas guardadas", (Rectangle){0, 260, W, 40}, 26, LIGHTGRAY);
    centrar_texto("1/2/3: dificultad     ESC: volver", (Rectangle){0, H - 75, W, 30}, 22, LIGHTGRAY);
}

static void dibujar_confetti(const Juego *j) {
    for (int k = 0; k < MAX_CONF; k++)
        DrawRectangle((int)j->conf[k].x, (int)j->conf[k].y, 6, 10, j->conf[k].c);
}

static void dibujar_victoria(const Juego *j) {
    const char *t1, *t2 = "R: repetir   ESC: menu";
    if (j->jug == 2)
        t1 = (j->pts[0] == j->pts[1]) ? "EMPATE" : (j->pts[0] > j->pts[1] ? "GANA J1" : "GANA J2");
    else if (!j->guardado) {
        t1 = TextFormat("Ganaste en %d intentos! Nombre: %s_", j->intentos, j->nombre);
        t2 = "Escribi tu nombre y apreta ENTER";
    } else
        t1 = TextFormat("Ganaste en %d intentos y %ds%s", j->intentos, (int)j->t,
                        j->nuevo ? " - NUEVO RECORD!" : "");
    DrawRectangle(0, H / 2 - 65, W, 130, Fade(BLACK, .85f));
    centrar_texto(t1, (Rectangle){0, H / 2 - 55, W, 60}, 28, GOLD);
    centrar_texto(t2, (Rectangle){0, H / 2 + 15, W, 40}, 20, RAYWHITE);
}

void dibujar_juego(const Juego *j) {
    if (j == NULL) return;

    BeginTextureMode(j->canvas);
    dibujar_fondo();
    
    if (j->pantalla == PANT_MENU) dibujar_menu(j);
    else if (j->pantalla == PANT_RANKING) dibujar_ranking(j);
    else {
        dibujar_hud(j);
        dibujar_tablero(j);
        
        if (j->pausa) {
            DrawRectangle(0, 0, W, H, Fade(BLACK, 0.8f));
            centrar_texto("PAUSA", (Rectangle){0, H / 2 - 35, W, 70}, 60, RAYWHITE);
        } else if (j->aciertos == j->filas * j->cols / 2) {
            dibujar_confetti(j);
            dibujar_victoria(j);
        }
    }
    EndTextureMode();

    float sw = (float)GetScreenWidth();
    float sh = (float)GetScreenHeight();
    float scale = fminf(sw / W, sh / H);
    float ox = (sw - W * scale) * 0.5f;
    float oy = (sh - H * scale) * 0.5f;

    BeginDrawing();
    ClearBackground(BLACK);
    DrawTexturePro(j->canvas.texture,
                   (Rectangle){0, 0, (float)j->canvas.texture.width, -(float)j->canvas.texture.height},
                   (Rectangle){ox, oy, W * scale, H * scale},
                   (Vector2){0, 0}, 0.0f, WHITE);
    EndDrawing();
}

#include "memoteFutbol.h"
#include <math.h>

static const int DIMS[3][2] = {{3, 4}, {4, 4}, {4, 6}};

Rectangle rect_carta(const Juego *j, int f, int col) {
    float x0 = (W - (j->cols * (CW + GAP) - GAP)) / 2;
    float y0 = TOP + (H - TOP - (j->filas * (CH + GAP) - GAP)) / 2;
    return (Rectangle){x0 + col * (CW + GAP), y0 + f * (CH + GAP), CW, CH};
}

Vector2 posicion_mouse(void) {
    Vector2 m = GetMousePosition();
    float sw = (float)GetScreenWidth();
    float sh = (float)GetScreenHeight();
    float scale = fminf(sw / W, sh / H);
    float ox = (sw - W * scale) * 0.5f;
    float oy = (sh - H * scale) * 0.5f;
    return (Vector2){(m.x - ox) / scale, (m.y - oy) / scale};
}

void inicializar_tablero(Juego *j) {
    int par[MAX_FILAS * MAX_COLS], total;
    if (j == NULL || j->dif < 0 || j->dif > 2) return;
    j->filas = DIMS[j->dif][0];
    j->cols = DIMS[j->dif][1];
    total = j->filas * j->cols;
    
    for (int i = 0; i < total; i++) par[i] = i / 2;
    for (int i = total - 1; i > 0; i--) {
        int k = GetRandomValue(0, i), aux = par[i];
        par[i] = par[k];
        par[k] = aux;
    }
    for (int i = 0; i < total; i++)
        j->c[i / j->cols][i % j->cols] = (Carta){par[i], 0, 0, 0};
}

int bonus_recursivo(int racha) {
    if (racha <= 1) return 100;
    return racha * 50 + bonus_recursivo(racha - 1);
}

static int total_pares(const Juego *j) { return j->filas * j->cols / 2; }
static int partida_ganada(const Juego *j) { return j->aciertos == total_pares(j); }

static int escribiendo_nombre(const Juego *j) {
    return j->pantalla == PANT_JUEGO && partida_ganada(j) && !j->guardado;
}

static void reiniciar_partida(Juego *j) {
    inicializar_tablero(j);
    liberar_pila(&j->pila);
    j->f1 = j->f2 = -1;
    j->intentos = j->aciertos = j->espera = j->racha = j->turno = 0;
    j->pts[0] = j->pts[1] = 0;
    j->nuevo = j->guardado = j->largo = 0;
    j->nombre[0] = '\0';
    j->t = 0;
    j->pausa = false;
}

static void animar_cartas(Juego *j) {
    float paso = 6 * GetFrameTime();
    for (int f = 0; f < j->filas; f++) {
        for (int col = 0; col < j->cols; col++) {
            Carta *k = &j->c[f][col];
            float meta = (k->estado != 0) ? 1.0f : 0.0f;
            k->flip += fmaxf(-paso, fminf(paso, meta - k->flip));
            if (k->shake > 0) k->shake--;
        }
    }
}

static void terminar_partida(Juego *j) {
    pedir_sonido(j, SND_WIN);
    for (int k = 0; k < MAX_CONF; k++) {
        j->conf[k] = (Conf){GetRandomValue(0, W), GetRandomValue(-H, 0), GetRandomValue(2, 5),
                            ColorFromHSV(GetRandomValue(0, 360), .8f, 1)};
    }
    j->guardado = (j->jug != 1);
}

static void registrar_acierto(Juego *j, Carta *a, Carta *b) {
    a->estado = b->estado = 2;
    j->f1 = j->f2 = -1;
    j->racha++;
    j->pts[j->turno] += bonus_recursivo(j->racha);
    pedir_sonido(j, SND_OK);
    if (++j->aciertos == total_pares(j)) terminar_partida(j);
}

static void registrar_error(Juego *j, Carta *a, Carta *b) {
    a->shake = b->shake = 20;
    j->espera = 60;
    j->racha = 0;
    pedir_sonido(j, SND_ERROR);
    if (j->jug == 2) j->turno = !j->turno;
}

static void evaluar_pareja(Juego *j) {
    Carta *a = &j->c[j->f1][j->c1], *b = &j->c[j->f2][j->c2];
    int acerto = (a->par == b->par);
    Movimiento m = {j->f1, j->c1, j->f2, j->c2, acerto, j->racha, j->turno, {j->pts[0], j->pts[1]}};
    apilar(&j->pila, m);
    if (acerto) registrar_acierto(j, a, b);
    else registrar_error(j, a, b);
}

static void voltear_carta(Juego *j, int f, int col) {
    j->c[f][col].estado = 1;
    pedir_sonido(j, SND_FLIP);
    if (j->f1 < 0) {
        j->f1 = f;
        j->c1 = col;
        return;
    }
    j->f2 = f;
    j->c2 = col;
    j->intentos++;
    evaluar_pareja(j);
}

static void manejar_clic(Juego *j) {
    Vector2 m = posicion_mouse();
    for (int f = 0; f < j->filas; f++) {
        for (int col = 0; col < j->cols; col++) {
            if (j->c[f][col].estado != 0) continue;
            if (!CheckCollisionPointRec(m, rect_carta(j, f, col))) continue;
            voltear_carta(j, f, col);
            return;
        }
    }
}

static void ocultar_seleccion(Juego *j) {
    if (j->f1 < 0 || j->f2 < 0) return;
    j->c[j->f1][j->c1].estado = 0;
    j->c[j->f2][j->c2].estado = 0;
    j->f1 = j->f2 = -1;
}

static void deshacer_movimiento(Juego *j) {
    Movimiento m;
    if (j->f1 >= 0 && j->espera == 0) return;
    if (!desapilar(&j->pila, &m)) return;
    j->c[m.f1][m.c1].estado = 0;
    j->c[m.f2][m.c2].estado = 0;
    if (m.acerto) j->aciertos--;
    j->racha = m.racha;
    j->turno = m.turno;
    j->pts[0] = m.pts[0];
    j->pts[1] = m.pts[1];
    j->f1 = j->f2 = -1;
    j->espera = 0;
}

static void actualizar_menu(Juego *j) {
    if (IsKeyPressed(KEY_ESCAPE)) j->salir = 1;
    for (int k = 0; k < 3; k++)
        if (IsKeyPressed(KEY_ONE + k)) j->dif = k;
    if (IsKeyPressed(KEY_P)) j->jug = 3 - j->jug;
    if (IsKeyPressed(KEY_T)) j->pantalla = PANT_RANKING;
    if (IsKeyPressed(KEY_ENTER)) {
        j->pantalla = PANT_JUEGO;
        reiniciar_partida(j);
    }
}

static void actualizar_ranking(Juego *j) {
    for (int k = 0; k < 3; k++)
        if (IsKeyPressed(KEY_ONE + k)) j->dif = k;
    if (IsKeyPressed(KEY_ESCAPE) || IsKeyPressed(KEY_ENTER)) j->pantalla = PANT_MENU;
}

static void guardar_resultado(Juego *j) {
    int mejor = mejor_de(j->ranking, j->dif);
    j->nuevo = (mejor == 0 || j->intentos < mejor);
    if (insertar_ranking(&j->ranking, j->nombre, j->dif, j->intentos))
        guardar_ranking(j->ranking);
    j->guardado = 1;
}

static void actualizar_final(Juego *j) {
    int ch;
    for (int k = 0; k < MAX_CONF; k++) {
        j->conf[k].y += j->conf[k].v;
        if (j->conf[k].y > H) j->conf[k].y = -10;
    }
    if (j->guardado) return;
    while ((ch = GetCharPressed()) > 0) {
        if (ch > 32 && ch < 127 && j->largo < NOMBRE_MAX - 1) {
            j->nombre[j->largo++] = (char)ch;
            j->nombre[j->largo] = '\0';
        }
    }
    if (IsKeyPressed(KEY_BACKSPACE) && j->largo > 0) j->nombre[--j->largo] = '\0';
    if (IsKeyPressed(KEY_ENTER) && j->largo > 0) guardar_resultado(j);
}

static void actualizar_partida(Juego *j) {
    if (IsKeyPressed(KEY_ESCAPE)) {
        j->pantalla = PANT_MENU;
        return;
    }
    if (IsKeyPressed(KEY_R) && !escribiendo_nombre(j)) reiniciar_partida(j);
    
    if (IsKeyPressed(KEY_P) && !escribiendo_nombre(j)) j->pausa = !j->pausa;
    if (j->pausa) return;

    animar_cartas(j);
    if (partida_ganada(j)) {
        actualizar_final(j);
        return;
    }
    
    j->t += GetFrameTime();
    if (IsKeyPressed(KEY_Z)) deshacer_movimiento(j);
    if (j->espera > 0 && --j->espera == 0) ocultar_seleccion(j);
    if (j->espera == 0 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) manejar_clic(j);
}

void iniciar_juego(Juego *j) {
    if (j == NULL) return;
    *j = (Juego){.pantalla = PANT_MENU, .dif = 1, .jug = 1, .f1 = -1, .f2 = -1, .sonando = -1, .pausa = false};
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(W, H, "MemoteFutbol");
    InitAudioDevice();
    SetExitKey(KEY_NULL);
    SetTargetFPS(60);
    j->canvas = LoadRenderTexture(W, H);
    SetTextureFilter(j->canvas.texture, TEXTURE_FILTER_BILINEAR);
    cargar_sonidos(j);
    cargar_imagenes(j);
    cargar_ranking(&j->ranking);
    inicializar_tablero(j);
}

void actualizar_juego(Juego *j) {
    if (j == NULL) return;
    if (IsKeyPressed(KEY_M) && !escribiendo_nombre(j))
        SetMasterVolume((j->mudo = !j->mudo) ? 0 : 1);
    if ((IsKeyPressed(KEY_F) || IsKeyPressed(KEY_F11)) && !escribiendo_nombre(j))
        ToggleFullscreen();
    switch (j->pantalla) {
        case PANT_MENU: actualizar_menu(j); break;
        case PANT_RANKING: actualizar_ranking(j); break;
        case PANT_JUEGO: actualizar_partida(j); break;
    }
    procesar_sonidos(j);
}

void cerrar_juego(Juego *j) {
    if (j == NULL) return;
    UnloadRenderTexture(j->canvas);
    liberar_sonidos(j);
    liberar_imagenes(j);
    liberar_pila(&j->pila);
    liberar_lista(&j->ranking);
    CloseAudioDevice();
    CloseWindow();
}

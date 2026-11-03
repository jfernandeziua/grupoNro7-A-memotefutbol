#include "memoteFutbol.h"

int main(void) {
    Juego j;
    iniciar_juego(&j);
    
    j.ambiente = LoadMusicStream("ambiente.mp3");
    PlayMusicStream(j.ambiente);
    
    while (!j.salir && !WindowShouldClose()) {
        UpdateMusicStream(j.ambiente);
        actualizar_juego(&j);
        dibujar_juego(&j);
    }
    
    UnloadMusicStream(j.ambiente);
    cerrar_juego(&j);
    return 0;
}
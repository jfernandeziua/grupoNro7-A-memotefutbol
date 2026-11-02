#include "memoteFutbol.h"

int main(void) {
    Juego j;
    iniciar_juego(&j);
    while (!j.salir && !WindowShouldClose()) {
        actualizar_juego(&j);
        dibujar_juego(&j);
    }
    cerrar_juego(&j);
    return 0;
}

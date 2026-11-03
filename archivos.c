#include "memoteFutbol.h"
#include <stdio.h>

void cargar_ranking(NodoLista **head) {
    char nombre[NOMBRE_MAX];
    int dif, intentos;
    FILE *f;
    if (head == NULL) return;
    f = fopen(ARCHIVO_RANKING, "r");
    if (f == NULL) return;   /* la primera vez el archivo todavia no existe */
    while (fscanf(f, "%d %d %10s", &dif, &intentos, nombre) == 3) {
        /* se ignoran las lineas con datos fuera de rango */
        if (dif >= 0 && dif <= 2 && intentos > 0)
            insertar_ranking(head, nombre, dif, intentos);
    }
    fclose(f);
}

/* Recorre la lista y la escribe completa en el archivo */
void guardar_ranking(const NodoLista *head) {
    const NodoLista *p;
    FILE *f = fopen(ARCHIVO_RANKING, "w");
    if (f == NULL) {
        TraceLog(LOG_WARNING, "No se pudo abrir %s para guardar", ARCHIVO_RANKING);
        return;   /* el juego sigue funcionando aunque no se pueda guardar */
    }
    for (p = head; p != NULL; p = p->sig)
        fprintf(f, "%d %d %s\n", p->dif, p->intentos, p->nombre);
    fclose(f);
}

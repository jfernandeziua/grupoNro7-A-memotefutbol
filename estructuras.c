#include "memoteFutbol.h"
#include <stdlib.h>
#include <string.h>


/* Push: crea un nodo en el heap y lo pone arriba de todo. Devuelve 1 si pudo. */
int apilar(NodoPila **tope, Movimiento m) {
    NodoPila *nuevo;
    if (tope == NULL) return 0;
    nuevo = (NodoPila *)malloc(sizeof(NodoPila));
    if (nuevo == NULL) return 0;
    nuevo->mov = m;
    nuevo->sig = *tope;
    *tope = nuevo;
    return 1;
}

/* Pop: saca el nodo de arriba, copia su contenido en *m y libera el nodo. */
int desapilar(NodoPila **tope, Movimiento *m) {
    NodoPila *aux;
    if (tope == NULL || *tope == NULL || m == NULL) return 0;
    aux = *tope;
    *m = aux->mov;
    *tope = aux->sig;
    free(aux);
    return 1;
}

void liberar_pila(NodoPila **tope) {
    Movimiento descarte;
    while (desapilar(tope, &descarte)) {
        /* desapilar libera de a un nodo hasta que la pila queda vacia */
    }
}

/* ================= LISTA ENLAZADA: ranking ordenado ================= */

/* Inserta ordenado por intentos (menor primero). Los empates quedan
   despues de los que ya estaban. Devuelve 1 si pudo insertar. */
int insertar_ranking(NodoLista **head, const char *nombre, int dif, int intentos) {
    NodoLista *nuevo, *ant = NULL, *act;
    if (head == NULL || nombre == NULL) return 0;
    nuevo = (NodoLista *)malloc(sizeof(NodoLista));
    if (nuevo == NULL) return 0;
    strncpy(nuevo->nombre, nombre, NOMBRE_MAX - 1);
    nuevo->nombre[NOMBRE_MAX - 1] = '\0';
    nuevo->dif = dif;
    nuevo->intentos = intentos;
    act = *head;
    while (act != NULL && act->intentos <= intentos) {
        ant = act;
        act = act->sig;
    }
    nuevo->sig = act;
    if (ant == NULL) *head = nuevo;
    else ant->sig = nuevo;
    return 1;
}

/* Mejor marca de una dificultad (la lista esta ordenada). 0 si no hay. */
int mejor_de(const NodoLista *head, int dif) {
    const NodoLista *p;
    for (p = head; p != NULL; p = p->sig)
        if (p->dif == dif) return p->intentos;
    return 0;
}

void liberar_lista(NodoLista **head) {
    NodoLista *aux;
    if (head == NULL) return;
    while (*head != NULL) {
        aux = *head;
        *head = aux->sig;
        free(aux);
    }
}

/* ================= COLA (FIFO): sonidos pendientes ================= */

/* Enqueue: agrega al final. */
int encolar(Cola *q, int tipo) {
    NodoCola *nuevo;
    if (q == NULL) return 0;
    nuevo = (NodoCola *)malloc(sizeof(NodoCola));
    if (nuevo == NULL) return 0;
    nuevo->tipo = tipo;
    nuevo->sig = NULL;
    if (q->final == NULL) q->frente = nuevo;   /* cola vacia */
    else q->final->sig = nuevo;
    q->final = nuevo;
    return 1;
}

/* Dequeue: saca del frente. Devuelve -1 si la cola esta vacia. */
int desencolar(Cola *q) {
    NodoCola *aux;
    int tipo;
    if (q == NULL || q->frente == NULL) return -1;
    aux = q->frente;
    tipo = aux->tipo;
    q->frente = aux->sig;
    if (q->frente == NULL) q->final = NULL;
    free(aux);
    return tipo;
}

void liberar_cola(Cola *q) {
    while (desencolar(q) >= 0) {
        /* vacia la cola liberando cada nodo */
    }
}

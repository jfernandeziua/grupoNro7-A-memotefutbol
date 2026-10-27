#include "memoteFutbol.h"
#include <math.h>

#define FREC_MUESTREO 44100

/* Genera una onda senoidal PCM de 16 bits. El buffer se reserva en el heap
   con MemAlloc y se libera despues con UnloadWave. Si no hay memoria,
   devuelve una onda vacia (data == NULL). */
Wave generar_tono(float freq, float dur) {
    Wave w = {0};
    int m = (int)(FREC_MUESTREO * dur);
    short *s;
    if (m <= 0) return w;
    s = (short *)MemAlloc(m * sizeof(short));
    if (s == NULL) return w;
    /* La amplitud baja linealmente a cero para que no suene un "clic" al final */
    for (int i = 0; i < m; i++)
        s[i] = (short)(sinf(2 * PI * freq * i / FREC_MUESTREO) * 8000 * (1 - (float)i / m));
    w.frameCount = m;
    w.sampleRate = FREC_MUESTREO;
    w.sampleSize = 16;
    w.channels = 1;
    w.data = s;
    return w;
}

/* Crea los cuatro sonidos del juego a partir de tonos sintetizados */
void cargar_sonidos(Juego *j) {
    const float frec[CANT_SND] = {600, 900, 180, 1200};
    const float dur[CANT_SND] = {.08f, .25f, .3f, .6f};
    if (j == NULL) return;
    for (int i = 0; i < CANT_SND; i++) {
        Wave w = generar_tono(frec[i], dur[i]);
        if (w.data == NULL) continue;
        j->snd[i] = LoadSoundFromWave(w);
        UnloadWave(w);   /* LoadSoundFromWave copia los datos: liberamos el buffer */
    }
}

void liberar_sonidos(Juego *j) {
    if (j == NULL) return;
    for (int i = 0; i < CANT_SND; i++)
        if (j->snd[i].frameCount > 0) UnloadSound(j->snd[i]);
    liberar_cola(&j->cola);
}


#pragma once
#include <string>
using namespace std;

enum Screen {
    SCR_MENU,
    SCR_REG_TORRE,
    SCR_SHOW_TORRES,
    SCR_DEL_TORRE,
    SCR_REG_OLEADA,
    SCR_SHOW_OLEADAS,
    SCR_INICIAR_OLEADA,   // nueva: seleccionar oleada
    SCR_JUEGO,
    SCR_AVANZAR_TURNO,    // nueva: resumen del turno
    SCR_SHOW_ENEMIGOS,
    SCR_ESTADO,
    SCR_SALIR
};

struct Estado {
    int  vidas       = 3;
    int  dinero      = 200;
    int  puntuacion  = 0;
    int  turno       = 0;
    int  idTorre     = 1;
    int  idEnemigo   = 1;
    bool gameOver    = false;
    bool victoria    = false;
};

struct Campo {
    char buf[64] = "";
    int  len     = 0;
    bool activo  = false;

    void clear() { buf[0] = 0; len = 0; activo = false; }

    void update() {
        if (!activo) return;
        int k = GetCharPressed();
        while (k > 0) {
            if (k >= 32 && k < 127 && len < 62) { buf[len++] = (char)k; buf[len] = 0; }
            k = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE) && len > 0) buf[--len] = 0;
    }

    int   toInt()   { return atoi(buf); }
    float toFloat() { return (float)atof(buf); }
};

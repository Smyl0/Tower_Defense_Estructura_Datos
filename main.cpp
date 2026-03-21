#include "raylib.h"
#include "Estado.h"
#include "Helpers.h"
#include "GameScreen.h"
#include "Menu.h"
#include "ListaTorres.h"
#include "ListaEnemigos.h"
#include "ListaOleadas.h"

int main() {
    InitWindow(SW, SH, "Tower Defense — Estructura de Datos C++");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);

    ListaSecuencialTorres listaTorres;
    ListaDobleEnemigos    listaEnemigos;
    ListaCircularOleadas  listaOleadas;

    {
        Oleada o;
        o.idOleada=1;o.cantidadEnemigos=3;o.tipoEnemigo="Basico";o.vidaBase=50;o.velocidadBase=1;listaOleadas.registrar(o);
        o.idOleada=2;o.cantidadEnemigos=5;o.tipoEnemigo="Rapido";o.vidaBase=40;o.velocidadBase=2;listaOleadas.registrar(o);
        o.idOleada=3;o.cantidadEnemigos=7;o.tipoEnemigo="Tanque";o.vidaBase=150;o.velocidadBase=1;listaOleadas.registrar(o);
        o.idOleada=4;o.cantidadEnemigos=10;o.tipoEnemigo="Jefe";o.vidaBase=300;o.velocidadBase=1;listaOleadas.registrar(o);
    }

    Estado     estado;
    GameScreen game;
    Menu       menu;
    Screen     pantalla = SCR_MENU;

    while (!WindowShouldClose()) {
        float dt = GetFrameTime();
        if (pantalla == SCR_SALIR) break;

        if (pantalla == SCR_JUEGO) {
            Screen next = game.handleInput(dt, estado, listaTorres, listaEnemigos, listaOleadas);
            if (next == SCR_MENU) pantalla = SCR_MENU;
            else game.update(dt, estado, listaTorres, listaEnemigos, listaOleadas);
        }

        BeginDrawing();
        ClearBackground(C_BG);

        switch (pantalla) {
            case SCR_MENU:
                pantalla = menu.drawMenu(estado, game.nT, game.oleadaIdx, listaTorres, listaEnemigos, listaOleadas);
                if (pantalla != SCR_MENU) menu.onEnter(pantalla);
                break;
            case SCR_REG_TORRE:
                pantalla = menu.drawRegTorre(dt, estado, listaTorres, game);
                break;
            case SCR_SHOW_TORRES:
                pantalla = menu.drawShowTorres(listaTorres);
                break;
            case SCR_DEL_TORRE:
                pantalla = menu.drawDelTorre(dt, estado, listaTorres, game);
                break;
            case SCR_REG_OLEADA:
                pantalla = menu.drawRegOleada(dt, listaOleadas);
                break;
            case SCR_SHOW_OLEADAS:
                pantalla = menu.drawShowOleadas(game.oleadaIdx, listaOleadas);
                break;
            case SCR_INICIAR_OLEADA:
                pantalla = menu.drawIniciarOleada(dt, game, listaOleadas);
                break;
            case SCR_JUEGO:
                game.draw(estado, listaTorres, listaEnemigos, listaOleadas);
                if (game.resetJuego) {
                    game.reset(estado);
                    listaTorres = ListaSecuencialTorres();
                    listaEnemigos = ListaDobleEnemigos();
                    // Se preserva listaOleadas, pero se reinicia el puntero actual en el ciclo.
                    listaOleadas.reiniciar();
                    game.oleadaIdx = 0;
                    pantalla = SCR_MENU; // regresar a menú tras reiniciar
                }
                if (game.irMenu) {
                    game.irMenu = false;
                    pantalla = SCR_MENU;
                }
                break;
            case SCR_AVANZAR_TURNO:
                pantalla = menu.drawAvanzarTurno(dt, estado, listaTorres, listaEnemigos, listaOleadas, game);
                break;
            case SCR_SHOW_ENEMIGOS:
                pantalla = menu.drawShowEnemigos(listaEnemigos);
                break;
            case SCR_ESTADO:
                pantalla = menu.drawEstado(estado, game.oleadaIdx, listaTorres, listaEnemigos, listaOleadas);
                break;
            default: break;
        }

        EndDrawing();
    }

    menu.descargarRecursos();
    CloseWindow();
    return 0;
}

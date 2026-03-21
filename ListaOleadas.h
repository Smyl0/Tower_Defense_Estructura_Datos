#pragma once
#include <iostream>
#include <string>
using namespace std;

// ============================================================
//  ESTRUCTURA OLEADA
// ============================================================
struct Oleada {
    int    idOleada;
    int    cantidadEnemigos;
    string tipoEnemigo;
    int    vidaBase;
    int    velocidadBase;
};

// ============================================================
//  NODO PARA LISTA CIRCULAR SIMPLEMENTE ENLAZADA
// ============================================================
struct NodoOleada {
    Oleada     dato;
    NodoOleada* siguiente;

    NodoOleada(const Oleada& o) : dato(o), siguiente(nullptr) {}
};

// ============================================================
//  LISTA SIMPLEMENTE ENLAZADA CIRCULAR
//  con referencia al ÚLTIMO nodo (el último apunta al primero)
// ============================================================
class ListaCircularOleadas {
private:
    NodoOleada* ultimo;   // último nodo; ultimo->siguiente = primero
    NodoOleada* actual;   // apuntador a la oleada en curso
    int tamanio;

public:
    ListaCircularOleadas() : ultimo(nullptr), actual(nullptr), tamanio(0) {}

    ~ListaCircularOleadas() {
        if (!ultimo) return;
        NodoOleada* primero = ultimo->siguiente;
        NodoOleada* nodo = primero;
        do {
            NodoOleada* sig = nodo->siguiente;
            delete nodo;
            nodo = sig;
        } while (nodo != primero);
    }

    // Registrar nueva oleada (insertar al final del círculo)
    void registrar(const Oleada& o) {
        NodoOleada* nuevo = new NodoOleada(o);
        if (!ultimo) {
            nuevo->siguiente = nuevo;   // apunta a sí mismo
            ultimo = nuevo;
        } else {
            nuevo->siguiente = ultimo->siguiente; // nuevo→primero
            ultimo->siguiente = nuevo;            // ultimo→nuevo
            ultimo = nuevo;                       // nuevo es el último
        }
        if (!actual) actual = ultimo->siguiente; // apunta al primero
        tamanio++;
        cout << "[OK] Oleada " << o.idOleada << " (" << o.tipoEnemigo << ") registrada.\n";
    }

    // Mostrar todas las oleadas recorriendo el círculo
    void mostrar() const {
        if (!ultimo) {
            cout << "  (No hay oleadas registradas)\n";
            return;
        }
        cout << "+-----+---------+--------------+------+------+\n";
        cout << "| ID  | Enemigos| Tipo         | Vida | Vel  |\n";
        cout << "+-----+---------+--------------+------+------+\n";
        NodoOleada* primero = ultimo->siguiente;
        NodoOleada* nodo = primero;
        do {
            const Oleada& ol = nodo->dato;
            bool esActual = (nodo == actual);
            cout << "| " << ol.idOleada
                 << "   | " << ol.cantidadEnemigos
                 << "       | " << ol.tipoEnemigo.substr(0,12)
                 << string(max(0,(int)(12-ol.tipoEnemigo.size())), ' ')
                 << " | " << ol.vidaBase
                 << "  | " << ol.velocidadBase
                 << (esActual ? " <- ACTUAL" : "")
                 << "\n";
            nodo = nodo->siguiente;
        } while (nodo != primero);
        cout << "+-----+---------+--------------+------+------+\n";
    }

    // Avanzar a la siguiente oleada (circularmente)
    void avanzar() {
        if (!actual) { cout << "[Error] No hay oleadas.\n"; return; }
        actual = actual->siguiente;
        cout << "[OK] Avanzando a oleada " << actual->dato.idOleada
             << " (" << actual->dato.tipoEnemigo << ").\n";
    }

    // Reiniciar al ciclo (volver al primero)
    void reiniciar() {
        if (!ultimo) return;
        actual = ultimo->siguiente;
        cout << "[OK] Ciclo de oleadas reiniciado.\n";
    }

    // Obtener la oleada actual
    Oleada* getActual() {
        return actual ? &actual->dato : nullptr;
    }

    Oleada* getPorIndice(int idx) {
        if (!ultimo || idx < 0 || idx >= tamanio) return nullptr;
        NodoOleada* nodo = ultimo->siguiente;
        for (int i = 0; i < idx; i++) nodo = nodo->siguiente;
        return &nodo->dato;
    }

    int getTamanio() const { return tamanio; }
    bool estaVacia() const { return ultimo == nullptr; }
};
// NOTA: getPorIndice agregado para lectura visual

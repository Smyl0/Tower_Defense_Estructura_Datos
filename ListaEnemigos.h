#pragma once
#include <iostream>
#include <string>
using namespace std;

// ============================================================
//  ESTRUCTURA ENEMIGO
// ============================================================
struct Enemigo {
    int    id;
    string tipo;
    int    vida;
    int    velocidad;
    int    posicion;
    int    recompensa;
};

// ============================================================
//  NODO PARA LISTA DOBLEMENTE ENLAZADA
// ============================================================
struct NodoEnemigo {
    Enemigo     dato;
    NodoEnemigo* anterior;
    NodoEnemigo* siguiente;

    NodoEnemigo(const Enemigo& e)
        : dato(e), anterior(nullptr), siguiente(nullptr) {}
};

// ============================================================
//  LISTA DOBLEMENTE ENLAZADA CON REFERENCIA A PRIMERO Y ÚLTIMO
//  para enemigos activos en campo
// ============================================================
class ListaDobleEnemigos {
private:
    NodoEnemigo* primero;
    NodoEnemigo* ultimo;
    int tamanio;

public:
    ListaDobleEnemigos() : primero(nullptr), ultimo(nullptr), tamanio(0) {}

    ~ListaDobleEnemigos() {
        NodoEnemigo* actual = primero;
        while (actual) {
            NodoEnemigo* sig = actual->siguiente;
            delete actual;
            actual = sig;
        }
    }

    // Insertar enemigo al final
    void insertarAlFinal(const Enemigo& e) {
        NodoEnemigo* nuevo = new NodoEnemigo(e);
        if (!ultimo) {
            primero = ultimo = nuevo;
        } else {
            nuevo->anterior = ultimo;
            ultimo->siguiente = nuevo;
            ultimo = nuevo;
        }
        tamanio++;
    }

    // Eliminar enemigo por id
    bool eliminarPorId(int id) {
        NodoEnemigo* actual = primero;
        while (actual) {
            if (actual->dato.id == id) {
                if (actual->anterior) actual->anterior->siguiente = actual->siguiente;
                else primero = actual->siguiente;
                if (actual->siguiente) actual->siguiente->anterior = actual->anterior;
                else ultimo = actual->anterior;
                delete actual;
                tamanio--;
                return true;
            }
            actual = actual->siguiente;
        }
        return false;
    }

    // Buscar enemigo por id
    NodoEnemigo* buscarPorId(int id) {
        NodoEnemigo* actual = primero;
        while (actual) {
            if (actual->dato.id == id) return actual;
            actual = actual->siguiente;
        }
        return nullptr;
    }

    // Recorrer hacia adelante (primero → último)
    void recorrerAdelante() const {
        if (!primero) {
            cout << "  (No hay enemigos activos)\n";
            return;
        }
        cout << "  [Adelante] ";
        NodoEnemigo* actual = primero;
        while (actual) {
            cout << "[ID:" << actual->dato.id
                 << " Tipo:" << actual->dato.tipo
                 << " Vida:" << actual->dato.vida
                 << " Pos:" << actual->dato.posicion << "] ";
            actual = actual->siguiente;
        }
        cout << "\n";
    }

    // Recorrer hacia atrás (último → primero)
    void recorrerAtras() const {
        if (!ultimo) {
            cout << "  (No hay enemigos activos)\n";
            return;
        }
        cout << "  [Atrás]    ";
        NodoEnemigo* actual = ultimo;
        while (actual) {
            cout << "[ID:" << actual->dato.id
                 << " Tipo:" << actual->dato.tipo
                 << " Vida:" << actual->dato.vida
                 << " Pos:" << actual->dato.posicion << "] ";
            actual = actual->anterior;
        }
        cout << "\n";
    }

    // Mostrar tabla de enemigos activos
    void mostrar() const {
        if (!primero) {
            cout << "  (No hay enemigos activos en el campo)\n";
            return;
        }
        cout << "+----+--------------+------+-----+-----+------------+\n";
        cout << "| ID | Tipo         | Vida | Vel | Pos | Recompensa |\n";
        cout << "+----+--------------+------+-----+-----+------------+\n";
        NodoEnemigo* actual = primero;
        while (actual) {
            const Enemigo& e = actual->dato;
            cout << "| " << e.id
                 << "  | " << e.tipo.substr(0,12)
                 << string(max(0,(int)(12-e.tipo.size())), ' ')
                 << " | " << e.vida
                 << "  | " << e.velocidad
                 << "  | " << e.posicion
                 << "  | " << e.recompensa
                 << "        |\n";
            actual = actual->siguiente;
        }
        cout << "+----+--------------+------+-----+-----+------------+\n";
    }

    // Actualizar posición de todos los enemigos
    // Retorna cuántos llegaron al final (posicion >= 20)
    int avanzarTodos(int limiteRuta = 20) {
        int llegaron = 0;
        NodoEnemigo* actual = primero;
        while (actual) {
            actual->dato.posicion += actual->dato.velocidad;
            if (actual->dato.posicion >= limiteRuta)
                llegaron++;
            actual = actual->siguiente;
        }
        return llegaron;
    }

    NodoEnemigo* getPrimero() const { return primero; }
    NodoEnemigo* getUltimo()  const { return ultimo; }
    int getTamanio() const { return tamanio; }
    bool estaVacia() const { return primero == nullptr; }
};

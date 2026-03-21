#pragma once
#include <iostream>
#include <string>
using namespace std;

// ============================================================
//  ESTRUCTURA TORRE
// ============================================================
struct Torre {
    int    id;
    string nombre;
    string tipo;
    int    posicion;
    int    danio;
    int    rango;
    int    costo;
};

// ============================================================
//  LISTA SECUENCIAL MANUAL PARA TORRES
//  - Sin std::vector, sin STL de contenedores
//  - Array de capacidad fija con tamanio lógico
// ============================================================
class ListaSecuencialTorres {
private:
    static const int CAPACIDAD = 100;
    Torre datos[CAPACIDAD];
    int tamanio;

public:
    ListaSecuencialTorres() : tamanio(0) {}

    // Insertar torre al final
    bool insertar(const Torre& t) {
        if (tamanio >= CAPACIDAD) {
            cout << "[Error] Lista de torres llena.\n";
            return false;
        }
        datos[tamanio++] = t;
        cout << "[OK] Torre '" << t.nombre << "' registrada con ID " << t.id << ".\n";
        return true;
    }

    // Eliminar torre por id
    bool eliminarPorId(int id) {
        for (int i = 0; i < tamanio; i++) {
            if (datos[i].id == id) {
                for (int j = i; j < tamanio - 1; j++)
                    datos[j] = datos[j + 1];
                tamanio--;
                cout << "[OK] Torre con ID " << id << " eliminada.\n";
                return true;
            }
        }
        cout << "[Error] Torre con ID " << id << " no encontrada.\n";
        return false;
    }

    // Buscar torre por id (retorna puntero o nullptr)
    Torre* buscarPorId(int id) {
        for (int i = 0; i < tamanio; i++)
            if (datos[i].id == id)
                return &datos[i];
        return nullptr;
    }

    // Mostrar todas las torres
    void mostrar() const {
        if (tamanio == 0) {
            cout << "  (No hay torres registradas)\n";
            return;
        }
        cout << "+------+----------------+----------+-----+-------+-------+-------+\n";
        cout << "| ID   | Nombre         | Tipo     | Pos | Daño  | Rango | Costo |\n";
        cout << "+------+----------------+----------+-----+-------+-------+-------+\n";
        for (int i = 0; i < tamanio; i++) {
            const Torre& t = datos[i];
            cout << "| " << t.id
                 << "    | " << t.nombre.substr(0,14)
                 << string(max(0,(int)(14-t.nombre.size())), ' ')
                 << " | " << t.tipo.substr(0,8)
                 << string(max(0,(int)(8-t.tipo.size())), ' ')
                 << " | " << t.posicion
                 << "  | " << t.danio
                 << "   | " << t.rango
                 << "     | " << t.costo
                 << "   |\n";
        }
        cout << "+------+----------------+----------+-----+-------+-------+-------+\n";
    }

    // Contar torres activas
    int contar() const { return tamanio; }

    // Acceso al array para lógica del juego
    Torre& get(int i) { return datos[i]; }
    int getTamanio() const { return tamanio; }
};

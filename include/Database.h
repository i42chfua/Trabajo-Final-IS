#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <iostream>
#include <vector>
#include "sqlite3.h"  // libreria de sqlite en C++

using namespace std;

class Database {
private:
    sqlite3* db; // Puntero a la base de datos

public:

    // Constructor
    Database(const string& path);
    
    // Destructor: Cierra la conexion
    ~Database();

    // Devuelve el rol del usuario si existe, o string vacio si falla
    string validarLogin(string dni);

    // Funcion para ejecutar Queries de SQLite
    bool ejecutarQuery(string sql);
    
    // NOTA: Agregar aqui las funciones:
    // vector<Usuario> getListaAlumnos();
    // void guardarMensaje(...);
};

#endif

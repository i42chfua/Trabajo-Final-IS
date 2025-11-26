#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include "../lib/sqlite3.h" 
#include "Usuario.h"

using namespace std;

class Database {
private:
    sqlite3* db;
    // Metodo privado para ejecutar querys (INSERT, UPDATE)
    bool ejecutarQuery(string sql);

public:
    Database(const string& path);
    ~Database();

    // --- AUTENTICACIÓN ---
    // Devuelve un objeto Usuario completo si el DNI existe, o un Usuario con id=-1 si falla
    Usuario login(string dni);

    // --- PARTE DE ASIGNACIÓN AUTOMÁTICA ---
    // Busca un tutor libre y lo vincula al alumno. Devuelve booleano.
    bool asignarTutorAutomaticamente(int idAlumno);

    // --- MÉTODOS ---
    // Metodos para listar usuarios o guardar mensajes
};

#endif

#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include "../lib/sqlite3.h" 
#include "Usuario.h"

using namespace std;

// --- ESTRUCTURA NUEVA PARA EL CHAT ---

struct Mensaje {
    string nombreEmisor;
    string contenido;
    string fecha;
};
// -------------------------------------

class Database {
private:
    sqlite3* db;
    bool ejecutarQuery(string sql);

public:
    Database(const string& path);
    ~Database();

    Usuario login(string dni, string password);
    bool registrarUsuario(Usuario nuevoUsuario);
    bool asignarTutorAutomaticamente(int idAlumno);
    vector<Usuario> getAllAlumnos();
    vector<Usuario> getAllTutores();
    bool asignarTutorManual(int idAlumno, int idTutor);
    string getNombrePorID(int idUsuario);   

    // --- NUEVAS FUNCIONES CHAT ---
    bool enviarMensaje(int idEmisor, int idReceptor, string mensaje);
    vector<Mensaje> getHistorialChat(int idUsuario1, int idUsuario2);
};

#endif
#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include "../lib/sqlite3.h" 
#include "Usuario.h"

using namespace std;


// Estructura para el chat

struct Mensaje {
    string nombreEmisor;
    string contenido;
    string fecha;
};

class Database {
private:
    sqlite3* db;
    bool ejecutarQuery(string sql);

public:

    //Constructor
    Database(const string& path);

    //Destructor
    ~Database();

    // Login de usuarios
    Usuario login(string dni, string password);

    // Registro de usuarios
    bool registrarUsuario(Usuario nuevoUsuario);

    // Asignacion automatica de tutor a alumno
    bool asignarTutorAutomaticamente(int idAlumno);

    // Obtener lista de alumnos
    vector<Usuario> getAllAlumnos();

    // Obtener lista de todos los tutores (libres y ocupados)
    vector<Usuario> getAllTutores();

    // Asignar manualmente un alumno a un tutor
    bool asignarTutorManual(int idAlumno, int idTutor);

    // Funcion para poder ver el nombre de la persona que tutorizas o que tutoriza
    string getNombrePorID(int idUsuario);   

    bool enviarMensaje(int idEmisor, int idReceptor, string mensaje);
    vector<Mensaje> getHistorialChat(int idUsuario1, int idUsuario2);
};

#endif
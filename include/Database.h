#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include "../lib/sqlite3.h" 
#include "Usuario.h"

using namespace std;

class Database {
private:

    // Base de datos con SQLite
    sqlite3* db;
    bool ejecutarQuery(string sql);

public:

    // Constructor
    Database(const string& path);
    // Destructor
    ~Database();

    // Login de los usuarios
    Usuario login(string dni, string password);

    // Registro de los usuarios
    bool registrarUsuario(Usuario nuevoUsuario);

    // Asignacion automatica de tutor a alumno
    bool asignarTutorAutomaticamente(int idAlumno);
    
    // Obtener lista de alumnos sin tutor asignado
    vector<Usuario> getAlumnosSinTutor();
    
    // Obtener lista de tutores disponibles (sin alumnos)
    vector<Usuario> getTutoresDisponibles();

    // Asignar manualmente un alumno a un tutor
    bool asignarTutorManual(int idAlumno, int idTutor);
    
    // Funcion para poder ver el nombre de la persona que tutorizas o que te tutoriza
    string getNombrePorID(int idUsuario);   

};

#endif

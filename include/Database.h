#ifndef DATABASE_H
#define DATABASE_H

#include <string>
#include <vector>
#include "../lib/sqlite3.h" 
#include "Usuario.h"

using namespace std;

struct Mensaje {
    string nombreEmisor;
    string contenido;
    string fecha;
};

struct Alerta {
    string nombreEmisor;
    string contenido;
    string fecha;
    int leida; 
};

class Database {
private:
    sqlite3* db;
    bool ejecutarQuery(string sql);

public:
    //Constructor
    Database(const string& path);
    // Destructor
    ~Database();

    Usuario login(string dni, string password);
    bool registrarUsuario(Usuario nuevoUsuario);

    // Asignaciones
    bool asignarTutorAutomaticamente(int idAlumno);
    bool asignarTutorManual(int idAlumno, int idTutor);
    
    // Consultas
    vector<Usuario> getAllAlumnos();
    vector<Usuario> getAllTutores();
    string getNombrePorID(int idUsuario);   

    // Chat
    bool enviarMensaje(int idEmisor, int idReceptor, string mensaje);
    vector<Mensaje> getHistorialChat(int idUsuario1, int idUsuario2);
    bool borrarChat(int idUsuario1, int idUsuario2);

    // Alertas
    bool enviarAlerta(int idEmisor, int idReceptor, string contenido);
    vector<Alerta> obtenerAlertasPendientes(int idUsuario);
    vector<Alerta> getHistorialAlertas(int idUsuario); 
    void marcarAlertasComoLeidas(int idUsuario);

    // Metricas
    int getNumAlertasEnviadas(int idUsuario);
    int getNumAlertasRecibidas(int idUsuario);
    bool tieneChatActivo(int idUsuario);
};

#endif
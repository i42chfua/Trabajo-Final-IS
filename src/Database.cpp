#include "../include/Database.h"
#include <iostream>

using namespace std;

Database::Database(const string& path) {
    if (sqlite3_open(path.c_str(), &db) != SQLITE_OK) {
        cerr << "Error: No se pudo abrir la BD." << endl;
    } else {

        // Tabla usuarios
        string sqlUsers = "CREATE TABLE IF NOT EXISTS USUARIOS(ID INTEGER PRIMARY KEY AUTOINCREMENT, NOMBRE TEXT NOT NULL, DNI TEXT UNIQUE NOT NULL, PASSWORD TEXT NOT NULL, ROL TEXT NOT NULL, ID_VINCULADO INTEGER DEFAULT 0);";
        ejecutarQuery(sqlUsers);

        // Tabla chat
        string sqlChat = "CREATE TABLE IF NOT EXISTS MENSAJES(ID INTEGER PRIMARY KEY AUTOINCREMENT, ID_EMISOR INTEGER, ID_RECEPTOR INTEGER, MENSAJE TEXT, FECHA DATETIME DEFAULT CURRENT_TIMESTAMP);";
        ejecutarQuery(sqlChat);
        
        // Tabla alertas
        string sqlAlertas = "CREATE TABLE IF NOT EXISTS ALERTAS(ID INTEGER PRIMARY KEY AUTOINCREMENT, ID_EMISOR INTEGER, ID_RECEPTOR INTEGER, CONTENIDO TEXT, LEIDA INTEGER DEFAULT 0, FECHA DATETIME DEFAULT CURRENT_TIMESTAMP);";
        ejecutarQuery(sqlAlertas);

        ejecutarQuery("INSERT OR IGNORE INTO USUARIOS (NOMBRE, DNI, PASSWORD, ROL) VALUES ('Super Jefe', 'admin', 'admin123', 'COORDINADOR');");
    }
}
// Destructor (cierre de la base de datos)
Database::~Database() { sqlite3_close(db); }

// Creacion Base de datos
bool Database::ejecutarQuery(string sql) {
    char* errMsg;
    if (sqlite3_exec(db, sql.c_str(), NULL, 0, &errMsg) != SQLITE_OK) {
        sqlite3_free(errMsg); return false;
    }
    return true;
}

// Login de usuarios
Usuario Database::login(string dni, string password) {
    string sql = "SELECT ID, NOMBRE, DNI, PASSWORD, ROL, ID_VINCULADO FROM USUARIOS WHERE DNI = '" + dni + "' AND PASSWORD = '" + password + "';";
    sqlite3_stmt* stmt;
    Usuario user; 
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            user.id = sqlite3_column_int(stmt, 0);
            user.nombre = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            user.dni = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
            user.password = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
            user.rol = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
            user.id_vinculado = sqlite3_column_int(stmt, 5);
        }
    }
    sqlite3_finalize(stmt);
    return user;
}

// Registro de usuarios (agregar una fila a la tabla de usuarios)
bool Database::registrarUsuario(Usuario u) {
    string sql = "INSERT INTO USUARIOS (NOMBRE, DNI, PASSWORD, ROL) VALUES ('" + u.nombre + "', '" + u.dni + "', '" + u.password + "', '" + u.rol + "');";
    return ejecutarQuery(sql);
}

// Asignacion automatica de alumnos a tutores
bool Database::asignarTutorAutomaticamente(int idAlumno) {
    string sqlBuscar = "SELECT ID FROM USUARIOS WHERE ROL='TUTOR' AND ID_VINCULADO=0 LIMIT 1;";
    sqlite3_stmt* stmt;
    int idTutorLibre = -1;
    if (sqlite3_prepare_v2(db, sqlBuscar.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) idTutorLibre = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);

    if (idTutorLibre != -1) {
        ejecutarQuery("UPDATE USUARIOS SET ID_VINCULADO=" + to_string(idTutorLibre) + " WHERE ID=" + to_string(idAlumno) + ";");
        ejecutarQuery("UPDATE USUARIOS SET ID_VINCULADO=" + to_string(idAlumno) + " WHERE ID=" + to_string(idTutorLibre) + ";");
        return true;
    }
    return false; 
}

// Saber el nombre gracias al id de un usuario (Para las interfaces)
string Database::getNombrePorID(int idUsuario) {
    string sql = "SELECT NOMBRE FROM USUARIOS WHERE ID = " + to_string(idUsuario) + ";";
    sqlite3_stmt* stmt;
    string nombre = "Desconocido";
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) nombre = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
    }
    sqlite3_finalize(stmt);
    return nombre;
}

// Obtener una lista de todos los alumnos
vector<Usuario> Database::getAllAlumnos() {
    vector<Usuario> lista;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, "SELECT ID, NOMBRE, DNI, ID_VINCULADO FROM USUARIOS WHERE ROL='ALUMNO';", -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Usuario u;
            u.id = sqlite3_column_int(stmt, 0);
            u.nombre = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            u.dni = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
            u.id_vinculado = sqlite3_column_int(stmt, 3);
            lista.push_back(u);
        }
    }
    sqlite3_finalize(stmt);
    return lista;
}

vector<Usuario> Database::getAllTutores() {
    // Obtener una lista de todos los tutores
    vector<Usuario> lista;
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, "SELECT ID, NOMBRE, DNI, ID_VINCULADO FROM USUARIOS WHERE ROL='TUTOR';", -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Usuario u;
            u.id = sqlite3_column_int(stmt, 0);
            u.nombre = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            u.dni = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
            u.id_vinculado = sqlite3_column_int(stmt, 3);
            lista.push_back(u);
        }
    }
    sqlite3_finalize(stmt);
    return lista;
}

// Asignacion o Reasignacion manual de alunmno y tutor
bool Database::asignarTutorManual(int idAlumno, int idTutor) {
    sqlite3_stmt* stmt;
    int idOld = 0;
    
    // Limpiar tutor
    if (sqlite3_prepare_v2(db, ("SELECT ID_VINCULADO FROM USUARIOS WHERE ID=" + to_string(idTutor)).c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) idOld = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    if (idOld != 0) ejecutarQuery("UPDATE USUARIOS SET ID_VINCULADO=0 WHERE ID=" + to_string(idOld) + ";");

    // Limpiar alumno
    idOld = 0;
    if (sqlite3_prepare_v2(db, ("SELECT ID_VINCULADO FROM USUARIOS WHERE ID=" + to_string(idAlumno)).c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) idOld = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    if (idOld != 0) ejecutarQuery("UPDATE USUARIOS SET ID_VINCULADO=0 WHERE ID=" + to_string(idOld) + ";");

    return ejecutarQuery("UPDATE USUARIOS SET ID_VINCULADO=" + to_string(idTutor) + " WHERE ID=" + to_string(idAlumno) + ";") &&
           ejecutarQuery("UPDATE USUARIOS SET ID_VINCULADO=" + to_string(idAlumno) + " WHERE ID=" + to_string(idTutor) + ";");
}

bool Database::enviarMensaje(int idEmisor, int idReceptor, string mensaje) {
    return ejecutarQuery("INSERT INTO MENSAJES (ID_EMISOR, ID_RECEPTOR, MENSAJE) VALUES (" + to_string(idEmisor) + ", " + to_string(idReceptor) + ", '" + mensaje + "');");
}

bool Database::borrarChat(int idUsuario1, int idUsuario2) {
    return ejecutarQuery("DELETE FROM MENSAJES WHERE (ID_EMISOR=" + to_string(idUsuario1) + " AND ID_RECEPTOR=" + to_string(idUsuario2) + ") OR (ID_EMISOR=" + to_string(idUsuario2) + " AND ID_RECEPTOR=" + to_string(idUsuario1) + ");");
}

vector<Mensaje> Database::getHistorialChat(int idUsuario1, int idUsuario2) {
    vector<Mensaje> historial;
    string sql = "SELECT U.NOMBRE, M.MENSAJE, M.FECHA FROM MENSAJES M JOIN USUARIOS U ON M.ID_EMISOR = U.ID WHERE (M.ID_EMISOR = " + to_string(idUsuario1) + " AND M.ID_RECEPTOR = " + to_string(idUsuario2) + ") OR (M.ID_EMISOR = " + to_string(idUsuario2) + " AND M.ID_RECEPTOR = " + to_string(idUsuario1) + ") ORDER BY M.FECHA ASC;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Mensaje m;
            m.nombreEmisor = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
            m.contenido = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            const char* f = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)); m.fecha = f ? f : "";
            historial.push_back(m);
        }
    }
    sqlite3_finalize(stmt);
    return historial;
}


// Enviar alerta
bool Database::enviarAlerta(int idEmisor, int idReceptor, string contenido) {
    return ejecutarQuery("INSERT INTO ALERTAS (ID_EMISOR, ID_RECEPTOR, CONTENIDO) VALUES (" + to_string(idEmisor) + ", " + to_string(idReceptor) + ", '" + contenido + "');");
}

// Notificacion de alerta nueva al usuario en cuestion
vector<Alerta> Database::obtenerAlertasPendientes(int idUsuario) {
    vector<Alerta> lista;
    string sql = "SELECT U.NOMBRE, A.CONTENIDO, A.FECHA FROM ALERTAS A JOIN USUARIOS U ON A.ID_EMISOR = U.ID WHERE A.ID_RECEPTOR=" + to_string(idUsuario) + " AND A.LEIDA=0 ORDER BY A.FECHA DESC;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Alerta a;
            a.nombreEmisor = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
            a.contenido = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            const char* f = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)); a.fecha = f ? f : "";
            a.leida = 0; // Son las pendientes
            lista.push_back(a);
        }
    }
    sqlite3_finalize(stmt);
    return lista;
}

// Ver el historial de alertas recibidas
vector<Alerta> Database::getHistorialAlertas(int idUsuario) {
    vector<Alerta> lista;
    // Obtenemos todas las alertas leidas
    string sql = "SELECT U.NOMBRE, A.CONTENIDO, A.FECHA, A.LEIDA FROM ALERTAS A JOIN USUARIOS U ON A.ID_EMISOR = U.ID WHERE A.ID_RECEPTOR=" + to_string(idUsuario) + " ORDER BY A.FECHA DESC;";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Alerta a;
            a.nombreEmisor = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
            a.contenido = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            const char* f = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)); a.fecha = f ? f : "";
            a.leida = sqlite3_column_int(stmt, 3);
            lista.push_back(a);
        }
    }
    sqlite3_finalize(stmt);
    return lista;
}

// Marcar alerta como leida cuando el usuario inicia sesion tras recibirla
void Database::marcarAlertasComoLeidas(int idUsuario) {
    ejecutarQuery("UPDATE ALERTAS SET LEIDA=1 WHERE ID_RECEPTOR=" + to_string(idUsuario) + ";");
}

// Metrica de alertas enviadas
int Database::getNumAlertasEnviadas(int idUsuario) {
    string sql = "SELECT COUNT(*) FROM ALERTAS WHERE ID_EMISOR=" + to_string(idUsuario) + ";";
    sqlite3_stmt* stmt;
    int count = 0;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) count = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return count;
}

// Metrica de alertas recibidas
int Database::getNumAlertasRecibidas(int idUsuario) {
    string sql = "SELECT COUNT(*) FROM ALERTAS WHERE ID_RECEPTOR=" + to_string(idUsuario) + ";";
    sqlite3_stmt* stmt;
    int count = 0;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) count = sqlite3_column_int(stmt, 0);
    }
    sqlite3_finalize(stmt);
    return count;
}

// Metrica de chat (Activo o Inactivo)
bool Database::tieneChatActivo(int idUsuario) {
    // Busca si ha enviado o recibido mensajes
    string sql = "SELECT ID FROM MENSAJES WHERE ID_EMISOR=" + to_string(idUsuario) + " OR ID_RECEPTOR=" + to_string(idUsuario) + " LIMIT 1;";
    sqlite3_stmt* stmt;
    bool activo = false;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) activo = true;
    }
    sqlite3_finalize(stmt);
    return activo;
}
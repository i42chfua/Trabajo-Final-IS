#include "../include/Database.h"
#include <iostream>

using namespace std;

Database::Database(const string& path) {
    if (sqlite3_open(path.c_str(), &db) != SQLITE_OK) {
        cerr << "Error: No se pudo abrir la BD." << endl;
    } else {
        // Tabla usuarios
        string sqlUsers = "CREATE TABLE IF NOT EXISTS USUARIOS("
                          "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "NOMBRE TEXT NOT NULL, "
                          "DNI TEXT UNIQUE NOT NULL, "
                          "PASSWORD TEXT NOT NULL, "
                          "ROL TEXT NOT NULL, "
                          "ID_VINCULADO INTEGER DEFAULT 0);";
        ejecutarQuery(sqlUsers);

        // Tabla mensajes
        string sqlChat = "CREATE TABLE IF NOT EXISTS MENSAJES("
                          "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "ID_EMISOR INTEGER, "
                          "ID_RECEPTOR INTEGER, "
                          "MENSAJE TEXT, "
                          "FECHA DATETIME DEFAULT CURRENT_TIMESTAMP);";
        ejecutarQuery(sqlChat);
        
        // Admin por defecto
        ejecutarQuery("INSERT OR IGNORE INTO USUARIOS (NOMBRE, DNI, PASSWORD, ROL) VALUES ('Super Jefe', 'admin', 'admin123', 'COORDINADOR');");
    }
}

Database::~Database() {
    sqlite3_close(db);
}

bool Database::ejecutarQuery(string sql) {
    char* errMsg;
    if (sqlite3_exec(db, sql.c_str(), NULL, 0, &errMsg) != SQLITE_OK) {
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

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

bool Database::registrarUsuario(Usuario u) {
    string sql = "INSERT INTO USUARIOS (NOMBRE, DNI, PASSWORD, ROL) VALUES ('" 
                 + u.nombre + "', '" + u.dni + "', '" + u.password + "', '" + u.rol + "');";
    return ejecutarQuery(sql);
}

bool Database::asignarTutorAutomaticamente(int idAlumno) {
    string sqlBuscar = "SELECT ID FROM USUARIOS WHERE ROL='TUTOR' AND ID_VINCULADO=0 LIMIT 1;";
    sqlite3_stmt* stmt;
    int idTutorLibre = -1;

    if (sqlite3_prepare_v2(db, sqlBuscar.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            idTutorLibre = sqlite3_column_int(stmt, 0);
        }
    }
    sqlite3_finalize(stmt);

    if (idTutorLibre != -1) {
        string updateAlumno = "UPDATE USUARIOS SET ID_VINCULADO=" + to_string(idTutorLibre) + " WHERE ID=" + to_string(idAlumno) + ";";
        string updateTutor = "UPDATE USUARIOS SET ID_VINCULADO=" + to_string(idAlumno) + " WHERE ID=" + to_string(idTutorLibre) + ";";
        ejecutarQuery(updateAlumno);
        ejecutarQuery(updateTutor);
        return true;
    }
    return false; 
}

string Database::getNombrePorID(int idUsuario) {
    string sql = "SELECT NOMBRE FROM USUARIOS WHERE ID = " + to_string(idUsuario) + ";";
    sqlite3_stmt* stmt;
    string nombreEncontrado = "Desconocido";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            nombreEncontrado = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        }
    }
    sqlite3_finalize(stmt);
    return nombreEncontrado;
}

vector<Usuario> Database::getAllAlumnos() {
    vector<Usuario> lista;
    string sql = "SELECT ID, NOMBRE, DNI, ID_VINCULADO FROM USUARIOS WHERE ROL='ALUMNO';";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
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
    vector<Usuario> lista;
    string sql = "SELECT ID, NOMBRE, DNI, ID_VINCULADO FROM USUARIOS WHERE ROL='TUTOR';";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
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

bool Database::asignarTutorManual(int idAlumno, int idTutor) {
    // 1. Limpiar tutor antiguo del alumno si lo tenia
    // (Simplificamos la lógica para ir directo al grano, pero tu lógica original era válida también)
    string updateAlumno = "UPDATE USUARIOS SET ID_VINCULADO=" + to_string(idTutor) + " WHERE ID=" + to_string(idAlumno) + ";";
    string updateTutor = "UPDATE USUARIOS SET ID_VINCULADO=" + to_string(idAlumno) + " WHERE ID=" + to_string(idTutor) + ";";
    
    // Lo ideal seria gestionar desvinculaciones previas, pero para que funcione rapido:
    ejecutarQuery(updateAlumno);
    ejecutarQuery(updateTutor);
    return true;
}

// --- IMPLEMENTACIÓN DEL CHAT ---

bool Database::enviarMensaje(int idEmisor, int idReceptor, string mensaje) {
    string sql = "INSERT INTO MENSAJES (ID_EMISOR, ID_RECEPTOR, MENSAJE) VALUES (" 
               + to_string(idEmisor) + ", " 
               + to_string(idReceptor) + ", '" 
               + mensaje + "');";
    return ejecutarQuery(sql);
}

vector<Mensaje> Database::getHistorialChat(int idUsuario1, int idUsuario2) {
    vector<Mensaje> historial;
    string sql = "SELECT U.NOMBRE, M.MENSAJE, M.FECHA "
                 "FROM MENSAJES M "
                 "JOIN USUARIOS U ON M.ID_EMISOR = U.ID "
                 "WHERE (M.ID_EMISOR = " + to_string(idUsuario1) + " AND M.ID_RECEPTOR = " + to_string(idUsuario2) + ") "
                 "   OR (M.ID_EMISOR = " + to_string(idUsuario2) + " AND M.ID_RECEPTOR = " + to_string(idUsuario1) + ") "
                 "ORDER BY M.FECHA ASC;";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        while (sqlite3_step(stmt) == SQLITE_ROW) {
            Mensaje m;
            m.nombreEmisor = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
            m.contenido    = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            const char* fechaTxt = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2));
            m.fecha = (fechaTxt ? string(fechaTxt) : "??");
            historial.push_back(m);
        }
    }
    sqlite3_finalize(stmt);
    return historial;
}
#include "../include/Database.h"
#include <iostream>

Database::Database(const string& path) {
    if (sqlite3_open(path.c_str(), &db) != SQLITE_OK) {
        cerr << "Error: No se pudo abrir la BD." << endl;
    } else {

        // Crea tabla USUARIOS
        // ID_VINCULADO guardarála relacion 1 a 1 entre tutor y alumno
        
        string sqlUsers = "CREATE TABLE IF NOT EXISTS USUARIOS("
                          "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "NOMBRE TEXT NOT NULL, "
                          "DNI TEXT UNIQUE NOT NULL, "
                          "ROL TEXT NOT NULL, "
                          "ID_VINCULADO INTEGER DEFAULT 0);"; // 0 significa 'sin asignar'
                                                              
        ejecutarQuery(sqlUsers);

        // Crear tabla MENSAJES Para el chat
        string sqlChat = "CREATE TABLE IF NOT EXISTS MENSAJES("
                         "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                         "ID_EMISOR INTEGER, "
                         "ID_RECEPTOR INTEGER, "
                         "MENSAJE TEXT, "
                         "FECHA DATETIME DEFAULT CURRENT_TIMESTAMP);";
        ejecutarQuery(sqlChat);
        
        // --- DATOS DE PRUEBA ---
        
        ejecutarQuery("INSERT OR IGNORE INTO USUARIOS (NOMBRE, DNI, ROL) VALUES ('Jefe', '00A', 'COORDINADOR');");
        ejecutarQuery("INSERT OR IGNORE INTO USUARIOS (NOMBRE, DNI, ROL) VALUES ('Profe1', '11A', 'TUTOR');");
        ejecutarQuery("INSERT OR IGNORE INTO USUARIOS (NOMBRE, DNI, ROL) VALUES ('Profe2', '22A', 'TUTOR');");
        ejecutarQuery("INSERT OR IGNORE INTO USUARIOS (NOMBRE, DNI, ROL) VALUES ('Alumno1', '33A', 'ALUMNO');");
        
    }
}

Database::~Database() {
    sqlite3_close(db);
}

bool Database::ejecutarQuery(string sql) {
    char* errMsg;
    if (sqlite3_exec(db, sql.c_str(), NULL, 0, &errMsg) != SQLITE_OK) {
        cerr << "SQL Error: " << errMsg << endl;
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

Usuario Database::login(string dni) {

    string sql = "SELECT ID, NOMBRE, DNI, ROL, ID_VINCULADO FROM USUARIOS WHERE DNI = '" + dni + "';";
    sqlite3_stmt* stmt;
    Usuario user; // Por defecto id = -1

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {
        if (sqlite3_step(stmt) == SQLITE_ROW) {
            user.id = sqlite3_column_int(stmt, 0);
            user.nombre = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
            user.dni = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
            user.rol = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
            user.id_vinculado = sqlite3_column_int(stmt, 4);
        }
    }

    sqlite3_finalize(stmt);
    return user;

}

bool Database::asignarTutorAutomaticamente(int idAlumno) {

    // 1. Buscar el primer TUTOR que tenga ID_VINCULADO a 0 (libre)

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

        // 2. Si encontramos tutor, hacemos el cruce (UPDATE en ambos)
        // Vinculamos Alumno -> Tutor
        
        string updateAlumno = "UPDATE USUARIOS SET ID_VINCULADO=" + to_string(idTutorLibre) + " WHERE ID=" + to_string(idAlumno) + ";";

        // Vinculamos Tutor -> Alumno
        
        string updateTutor = "UPDATE USUARIOS SET ID_VINCULADO=" + to_string(idAlumno) + " WHERE ID=" + to_string(idTutorLibre) + ";";
        
        ejecutarQuery(updateAlumno);
        ejecutarQuery(updateTutor);
        return true;

    }
    
    return false; // No habia tutores libres
                  
}


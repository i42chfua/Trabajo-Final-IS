#include "../include/Database.h"
#include <iostream>

Database::Database(const string& path) {

    if (sqlite3_open(path.c_str(), &db) != SQLITE_OK) {
        cerr << "Error: No se pudo abrir la BD." << endl;
    } else {

    // Tabla de usuarios
    
        string sqlUsers = "CREATE TABLE IF NOT EXISTS USUARIOS("
                          "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
                          "NOMBRE TEXT NOT NULL, "
                          "DNI TEXT UNIQUE NOT NULL, "
                          "PASSWORD TEXT NOT NULL, "  // <--- NUEVA COLUMNA
                          "ROL TEXT NOT NULL, "
                          "ID_VINCULADO INTEGER DEFAULT 0);";
        ejecutarQuery(sqlUsers);

    // Tabla de mensajes
    
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

// Cerrar base de datos (Destructor)
Database::~Database() {
    sqlite3_close(db);
}


bool Database::ejecutarQuery(string sql) {
    char* errMsg;
    if (sqlite3_exec(db, sql.c_str(), NULL, 0, &errMsg) != SQLITE_OK) {
        // cerr << "SQL Error: " << errMsg << endl; 
        sqlite3_free(errMsg);
        return false;
    }
    return true;
}

Usuario Database::login(string dni, string password) {

    // Login de usuario mediante dni y contrasena
    
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

    // Registro de un nuevo de usuario en la tabla correspondiente de la base de datos.
    string sql = "INSERT INTO USUARIOS (NOMBRE, DNI, PASSWORD, ROL) VALUES ('" 
                 + u.nombre + "', '" + u.dni + "', '" + u.password + "', '" + u.rol + "');";
    
    return ejecutarQuery(sql);

}

bool Database::asignarTutorAutomaticamente(int idAlumno) {

    // Asignacion automatica de tutores a alumnos
    
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

// Funcion para indicar el nombre de la persona que tutorizas o te tutoriza.
string Database::getNombrePorID(int idUsuario) {

    string sql = "SELECT NOMBRE FROM USUARIOS WHERE ID = " + to_string(idUsuario) + ";";
    sqlite3_stmt* stmt;
    string nombreEncontrado = "Desconocido";

    if (sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt, NULL) == SQLITE_OK) {

        if (sqlite3_step(stmt) == SQLITE_ROW) {

            // La columna 0 es el NOMBRE
            nombreEncontrado = string(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        }
    }
    sqlite3_finalize(stmt);

    return nombreEncontrado;

}

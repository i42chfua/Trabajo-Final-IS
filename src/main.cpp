#include <iostream>
#include "../include/Database.h"
#include "../include/Usuario.h"

// Incluir aquí las cabeceras de tus compañeros cuando las creen
// #include "../include/GestorChat.h"
// #include "../include/GestorTutorias.h"

using namespace std;

 void mostrarMenuCoordinador(Usuario& user, Database& db) {
    int opcion = 0;
    do {
        cout << "\n--- MENU COORDINADOR (" << user.nombre << ") ---" << endl;
        cout << "1. Asignar Tutor manualmente" << endl;
        cout << "2. Volver (Cerrar Sesion)" << endl;
        cout << "Opcion: ";
        cin >> opcion; // <--- ESTO ES LO QUE FALTABA (Esperar input)

        switch (opcion) {
            case 1:
                cout << ">> Aqui ira la logica de asignar tutor (Compañero 1)" << endl;

                // gestorTutorias.asignarTutor(db);
                
                break;
            case 2:
                cout << "Cerrando sesion..." << endl;
                break;
            default:
                cout << "Opcion no valida." << endl;
        }
    } while (opcion != 2); 
}

void mostrarMenuTutor(Usuario& user, Database& db) {
    int opcion = 0;
    do {
        cout << "\n--- MENU TUTOR (" << user.nombre << ") ---" << endl;
        if (user.id_vinculado == 0) {

            cout << "AVISO: No tienes alumno asignado." << endl;

        } else {

            cout << "Tutorizando al alumno con ID: " << user.id_vinculado << endl;
            cout << "1. Abrir Chat con Alumno" << endl;

        }

        cout << "2. Volver" << endl;
        cout << "Opcion: ";
        cin >> opcion;

        if (opcion == 1 && user.id_vinculado != 0) {

             cout << ">> Aqui ira el chat (Compañero 2)" << endl;
             
        } else if (opcion != 2) {

             cout << "Opcion no valida." << endl;

        }
    } while (opcion != 2);
}

void mostrarMenuAlumno(Usuario& user, Database& db) {
    int opcion = 0;
    do {

        cout << "\n--- MENU ALUMNO (" << user.nombre << ") ---" << endl;
        
        // Logica de asignación automática (Tu parte)
        if (user.id_vinculado == 0) {

            cout << "(!) No tienes tutor. Buscando uno disponible..." << endl;

            if (db.asignarTutorAutomaticamente(user.id)) {

                cout << "¡EXITO! Se te ha asignado un tutor. Reinicia sesion para ver cambios." << endl;

                // Recargamos datos del usuario actualizados desde la BD
                
                user = db.login(user.dni); 

            } else {

                cout << "Lo sentimos, no hay tutores libres." << endl;

            }
        } 
        
        if (user.id_vinculado != 0) {

            cout << "Tu tutor es el ID: " << user.id_vinculado << endl;
            cout << "1. Abrir Chat con Tutor" << endl;

        }

        cout << "2. Volver" << endl;
        cout << "Opcion: ";
        cin >> opcion;

        if (opcion == 1 && user.id_vinculado != 0) {

             cout << ">> Aqui ira el chat" << endl;

        }

    } while (opcion != 2);
}

// -- Parte antigua (Backup por si acaso) --

/*

void mostrarMenuCoordinador(Usuario& user, Database& db) {
 
    cout << "\n--- MENU COORDINADOR (" << user.nombre << ") ---" << endl;
    cout << "1. Asignar Tutor manualmente" << endl;
    cout << "2. Volver" << endl;
    // Aquí iría la llamada al código de tu compañero 1
    cout << "Opcion: ";
}

void mostrarMenuTutor(Usuario& user, Database& db) {
    cout << "\n--- MENU TUTOR (" << user.nombre << ") ---" << endl;
    if (user.id_vinculado == 0) {
        cout << "AVISO: No tienes alumno asignado." << endl;
    } else {
        cout << "Tutorizando al alumno con ID: " << user.id_vinculado << endl;
        cout << "1. Abrir Chat con Alumno" << endl;
        // Aquí iría la llamada al código de tu compañero 2
    }
    cout << "2. Volver" << endl;
}

void mostrarMenuAlumno(Usuario& user, Database& db) {
    cout << "\n--- MENU ALUMNO (" << user.nombre << ") ---" << endl;
    
    // --- TU IMPLEMENTACIÓN: ASIGNACIÓN AUTOMÁTICA ---
    if (user.id_vinculado == 0) {
        cout << "Buscando tutor disponible..." << endl;
        if (db.asignarTutorAutomaticamente(user.id)) {
            cout << "¡ÉXITO! Se te ha asignado un tutor nuevo. Vuelve a iniciar sesión para actualizar." << endl;
            // Recargamos el usuario para ver el cambio
            user = db.login(user.dni); 
        } else {
            cout << "Lo sentimos, no hay tutores libres en este momento." << endl;
        }
    } else {
        cout << "Tu tutor asignado es ID: " << user.id_vinculado << endl;
        cout << "1. Abrir Chat con Tutor" << endl;
        // Aquí iría la llamada al código de tu compañero 2
    }
    cout << "2. Volver" << endl;
}

*/

int main() {
    Database db("db/tutorias.db");
    
    // Pequeño hack para meter datos iniciales si la DB está vacía
    // db.ejecutarQuery("INSERT OR IGNORE INTO USUARIOS (NOMBRE, DNI, ROL) VALUES ('Admin', '0000', 'COORDINADOR');");

    while (true) {

        cout << "\n=== SISTEMA DE GESTION DE TUTORIAS ===" << endl;
        cout << "Introduce tu DNI para entrar (o 'salir'): ";
        string dni;
        cin >> dni;

        if (dni == "salir") break;

        // 1. LOGIN (Tu parte)
        
        Usuario usuarioLogueado = db.login(dni);

        if (usuarioLogueado.id == -1) {
            cout << "Error: DNI no encontrado." << endl;
        } else {
            cout << "Bienvenido, " << usuarioLogueado.nombre << endl;
            
            // 2. DISTRIBUCIÓN POR ROLES (Común)
            
            if (usuarioLogueado.rol == "COORDINADOR") {

                mostrarMenuCoordinador(usuarioLogueado, db);

            } else if (usuarioLogueado.rol == "TUTOR") {

                mostrarMenuTutor(usuarioLogueado, db);

            } else if (usuarioLogueado.rol == "ALUMNO") {

                mostrarMenuAlumno(usuarioLogueado, db);

            }
        }
    }

    return 0;

}


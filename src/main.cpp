#include <iostream>
#include <limits> 
#include "../include/Database.h"
#include "../include/Usuario.h"

using namespace std;

void mostrarMenuCoordinador(Usuario& user, Database& db) {
    // Menu de coordinador
    int opcion = 0;

    do {

        cout << "\n--- MENU COORDINADOR (" << user.nombre << ") ---" << endl;
        cout << "1. Asignar Tutor manualmente" << endl;
        cout << "2. Volver (Cerrar Sesion)" << endl;
        cout << "Opcion: ";

        cin >> opcion;
        
        switch (opcion) {

            case 1:
                cout << ">> Logica Asignar Tutor" << endl;
                break;

            case 2: break;

            default: cout << "Opcion no valida." << endl;

        }

    } while (opcion != 2);
}

void mostrarMenuTutor(Usuario& user, Database& db) {
    // Menu de tutor
    int opcion = 0;
    do {
        cout << "\n--- MENU TUTOR (" << user.nombre << ") ---" << endl;
        
        if (user.id_vinculado == 0) {

            cout << "(!) AVISO: Actualmente no tienes ningun alumno asignado." << endl;

        } else {

            string nombreAlumno = db.getNombrePorID(user.id_vinculado);
            cout << "\n--> Estas tutorizando al alumno: " << nombreAlumno << endl;
            
            cout << "1. Abrir Chat con " << nombreAlumno << endl;

        }

        cout << "2. Cerrar Sesion" << endl;
        cout << "Opcion: ";
        cin >> opcion;

        if (opcion == 1 && user.id_vinculado != 0) {

             cout << ">> Abriendo chat..." << endl;
             // gestorChat.abrirChat(...); Funcion de chat
             
        } else if (opcion != 2) {

             cout << "Opcion no valida." << endl;

        }

    } while (opcion != 2);

}

void mostrarMenuAlumno(Usuario& user, Database& db) {

    int opcion = 0;

    do {
        cout << "\n--- MENU ALUMNO (" << user.nombre << ") ---" << endl;
        
        if (user.id_vinculado == 0) {

            if (db.asignarTutorAutomaticamente(user.id)) {

                cout << "INFO! Se te ha asignado tutor automaticamente." << endl;
                user = db.login(user.dni, user.password); 

            } else {

                cout << "(!) Buscando tutor... (No hay disponibles aun)" << endl;

            }
        }
        
        if (user.id_vinculado != 0) {

            string nombreTutor = db.getNombrePorID(user.id_vinculado);
            cout << "\n--> Tu tutor asignado es: " << nombreTutor << endl;
            
            cout << "1. Abrir Chat con " << nombreTutor << endl;

        }

        cout << "2. Cerrar Sesion" << endl;

        cout << "Opcion: ";
        cin >> opcion;
        
        // Logica del chat
        if (opcion == 1 && user.id_vinculado != 0) {

             cout << ">> Abriendo chat..." << endl;

        }

    } while (opcion != 2);
}

void realizarRegistro(Database& db) {
    // Registro de nuevos usuarios *con seguridad para tutores y coordinadores*

    string nombre, dni, pass, rol, token;
    int opcionRol;
    
    cout << "\n--- REGISTRO DE NUEVO USUARIO ---" << endl;
    
   cin.ignore(); 
    
    cout << "Nombre completo (ej. Juan Perez): ";
    getline(cin, nombre); 
    
    cout << "DNI: ";
    cin >> dni;
    
    cout << "Contraseña: ";
    cin >> pass;
    
    cout << "Selecciona el Rol:\n1. ALUMNO\n2. TUTOR\n3. COORDINADOR\n> ";
    cin >> opcionRol;
    
    if (opcionRol == 1) {
        rol = "ALUMNO";
    } 
    else if (opcionRol == 2) {

        // SEGURIDAD PARA TUTORES
        cout << "Introduce el codigo de autorizacion para TUTORES: ";
        cin >> token;
        if (token == "tutoresUCO") { 
            rol = "TUTOR";
        } else {
            cout << "ERROR: Codigo incorrecto. No tienes permiso para ser Tutor." << endl;
            return; 
        }
    } 
    else if (opcionRol == 3) {

        // SEGURIDAD PARA COORDINADORES
        cout << "Introduce el codigo de autorizacion para COORDINADORES: ";
        cin >> token;
        if (token == "coordinadoresUCO") { 
            rol = "COORDINADOR";
        } else {
             cout << "ERROR: Codigo incorrecto. Intento de acceso no autorizado." << endl;
             return;
        }
    } 
    else {
        cout << "Rol no valido. Registro cancelado." << endl;
        return;
    }
    
    Usuario nuevo(0, nombre, dni, pass, rol, 0);
    
    if (db.registrarUsuario(nuevo)) {
        cout << "¡Registro exitoso como " << rol << "! Ahora puedes iniciar sesion." << endl;
        
        // Asignacion automatica para alumnos
        if (rol == "ALUMNO") {
             Usuario temp = db.login(dni, pass);
             db.asignarTutorAutomaticamente(temp.id);
        }
    } else {
        cout << "Error al registrar. Puede que el DNI ya exista." << endl;
    }
}

// --- MAIN PRINCIPAL ---
int main() {
    Database db("db/tutorias.db");
    
    int opcionInicio = 0;
    
    while (true) {

        cout << "\n=======================================" << endl;
        cout << "   BIENVENIDO AL GESTOR DE TUTORIAS    " << endl;
        cout << "=======================================\n" << endl;

        cout << "1. Iniciar Sesion" << endl;
        cout << "2. Registrarse" << endl;
        cout << "3. Salir del programa" << endl;
        cout << "Elige una opcion: ";


        if (!(cin >> opcionInicio)) {         

                cout << "Error: Debes introducir un numero valido." << endl;
                cin.clear();                      
                cin.ignore(10000, '\n');          
                continue;                        

        }
        
        if (opcionInicio == 3) {

            cout << "Saliendo..." << endl;
            break;

        }
        
        if (opcionInicio == 2) {

            realizarRegistro(db);

        }  else if (opcionInicio == 1) {

            string dni, pass;
            cout << "\n--- INICIO DE SESION ---" << endl;
            cout << "DNI: ";
            cin >> dni;
            cout << "Contraseña: ";
            cin >> pass;
            
            Usuario usuarioLogueado = db.login(dni, pass);
            
            if (usuarioLogueado.id == -1) {

                cout << "Error: DNI o Contraseña incorrectos. Intentalo de nuevo." << endl;

            } else {

                if (usuarioLogueado.rol == "COORDINADOR") {

                    mostrarMenuCoordinador(usuarioLogueado, db);

                } else if (usuarioLogueado.rol == "TUTOR") {

                    mostrarMenuTutor(usuarioLogueado, db);

                } else if (usuarioLogueado.rol == "ALUMNO") {

                    mostrarMenuAlumno(usuarioLogueado, db);

                }
            }

        } else {

            cout << "Opcion no valida." << endl;

        }
    }

    return 0;

}

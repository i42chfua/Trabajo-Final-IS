#include <iostream>
#include <limits> 
#include <vector>
#include <string>

#include "../include/Database.h"
#include "../include/Usuario.h"

using namespace std;

// --- FUNCIÓN DE CHAT ---
void pantallaDeChat(Usuario& yo, int idOtroUsuario, Database& db) {
    string input;
    string nombreOtro = db.getNombrePorID(idOtroUsuario);

    while (true) {
        cout << string(50, '\n'); // Limpiar pantalla
        
        cout << "========================================" << endl;
        cout << "   CHAT CON: " << nombreOtro << endl;
        cout << "========================================" << endl;

        vector<Mensaje> historial = db.getHistorialChat(yo.id, idOtroUsuario);
        
        if (historial.empty()) {
            cout << "  (No hay mensajes previos. Saluda!)" << endl;
        } else {
            for (Mensaje m : historial) {
                cout << "[" << m.fecha << "] " << m.nombreEmisor << ": " << m.contenido << endl;
            }
        }
        cout << "========================================" << endl;
        cout << "[Escribe mensaje y pulsa ENTER]" << endl;
        cout << "[Escribe 'SALIR' para volver]" << endl;
        cout << "> ";

        if (cin.peek() == '\n') cin.ignore(); 
        getline(cin, input);

        if (input == "SALIR" || input == "salir") break; 

        if (!input.empty()) {
            db.enviarMensaje(yo.id, idOtroUsuario, input);
        }
    }
}

void mostrarMenuCoordinador(Usuario& user, Database& db) {
    int opcion = 0;
    do {
        cout << "\n--- MENU COORDINADOR (" << user.nombre << ") ---" << endl;
        cout << "1. Asignar Tutor manualmente" << endl;
        cout << "2. Volver" << endl;
        cout << "Opcion: ";
        if (!(cin >> opcion)) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); continue; }

        if (opcion == 1) {
             vector<Usuario> alumnos = db.getAllAlumnos();
             cout << "--- Alumnos ---" << endl;
             for(auto& a : alumnos) cout << a.id << ": " << a.nombre << " (Vinc: " << a.id_vinculado << ")" << endl;
             
             vector<Usuario> tutores = db.getAllTutores();
             cout << "--- Tutores ---" << endl;
             for(auto& t : tutores) cout << t.id << ": " << t.nombre << " (Vinc: " << t.id_vinculado << ")" << endl;

             int idA, idT;
             cout << "\nID Alumno: "; cin >> idA;
             cout << "ID Tutor: "; cin >> idT;
             
             if(db.asignarTutorManual(idA, idT)){
                 cout << "Asignacion correcta." << endl;
             } else {
                 cout << "Error al asignar." << endl;
             }
        }
    } while (opcion != 2);
}

void mostrarMenuTutor(Usuario& user, Database& db) {
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
             pantallaDeChat(user, user.id_vinculado, db);
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
        
        if (opcion == 1 && user.id_vinculado != 0) {
             pantallaDeChat(user, user.id_vinculado, db);
        }

    } while (opcion != 2);
}

void realizarRegistro(Database& db) {
    string nombre, dni, pass, rol, token;
    int opcionRol;
    
    cout << "\nREGISTRO DE USUARIO" << endl;
    cout << "Nombre: "; 
    cin.ignore(); 
    getline(cin, nombre);
    
    cout << "DNI: "; cin >> dni;
    cout << "Pass: "; cin >> pass;
    cout << "Rol (1.Alumno, 2.Tutor, 3.Coord): "; cin >> opcionRol;
    
    if (opcionRol == 1) rol = "ALUMNO";
    else if (opcionRol == 2) {
        cout << "Token Tutor: "; cin >> token;
        if(token=="tutoresUCO") rol="TUTOR"; else { cout << "Token invalido." << endl; return; }
    }
    else if (opcionRol == 3) {
        cout << "Token Coord: "; cin >> token;
        if(token=="coordinadoresUCO") rol="COORDINADOR"; else { cout << "Token invalido." << endl; return; }
    } else return;

    Usuario nuevo(0, nombre, dni, pass, rol, 0);
    if(db.registrarUsuario(nuevo)){
        cout << "Registrado OK." << endl;
        if(rol == "ALUMNO") {
             Usuario temp = db.login(dni, pass);
             db.asignarTutorAutomaticamente(temp.id);
        }
    } else {
        cout << "Error: DNI duplicado." << endl;
    }
}

int main() {
    Database db("db/tutorias.db"); 
    int opcionInicio = 0;
    while (true) {
        cout << "\n=== GESTOR DE TUTORIAS UCO ===" << endl;
        cout << "1. Iniciar sesión\n2. Registrarse\n3. Salir del programa\n> ";
        if (!(cin >> opcionInicio)) { cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); continue; }
        
        if (opcionInicio == 3) break;
        if (opcionInicio == 2) realizarRegistro(db);
        else if (opcionInicio == 1) {
            string dni, pass;
            cout << "DNI: "; cin >> dni;
            cout << "Contraseña: "; cin >> pass;
            
            Usuario u = db.login(dni, pass);
            if (u.id != -1) {
                if (u.rol == "COORDINADOR") mostrarMenuCoordinador(u, db);
                else if (u.rol == "TUTOR") mostrarMenuTutor(u, db);
                else if (u.rol == "ALUMNO") mostrarMenuAlumno(u, db);
            } else cout << "DNI o Contraseña incorrecta/s." << endl;
        }
    }
    return 0;
}
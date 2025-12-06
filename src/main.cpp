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
        cout << string(50, '\n'); // "Limpiar" pantalla simulado
        
        cout << "========================================" << endl;
        cout << "   CHAT CON: " << nombreOtro << endl;
        cout << "========================================" << endl;

        vector<Mensaje> historial = db.getHistorialChat(yo.id, idOtroUsuario);
        
        if (historial.empty()) {
            cout << "  (No hay mensajes previos. Di hola!)" << endl;
        } else {
            for (Mensaje m : historial) {
                // Formato: [Fecha] Emisor: Mensaje
                cout << "[" << m.fecha << "] " << m.nombreEmisor << ": " << m.contenido << endl;
            }
        }
        cout << "========================================" << endl;
        cout << "[Escribe tu mensaje y pulsa ENTER]" << endl;
        cout << "[Escribe 'SALIR' para volver]" << endl;
        cout << "> ";

        getline(cin, input);

        if (input == "SALIR" || input == "salir") break; 

        if (!input.empty()) {
            db.enviarMensaje(yo.id, idOtroUsuario, input);
        }
    }
}

// --- MENÚS ---

void mostrarMenuCoordinador(Usuario& user, Database& db) {
    int opcion = 0;
    do {
        cout << "\n--- MENU COORDINADOR (" << user.nombre << ") ---" << endl;
        cout << "1. Asignar Tutor manualmente" << endl;
        cout << "2. Volver (Cerrar Sesion)" << endl;
        cout << "Opcion: ";
        
        if (!(cin >> opcion)) { 
            cin.clear(); cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            cout << "Entrada invalida." << endl; continue; 
        }

        if (opcion == 1) {
             cout << "\n>> ASIGNACION MANUAL DE TUTORIAS" << endl;

             // --- 1. LISTAR ALUMNOS ---
             vector<Usuario> alumnos = db.getAllAlumnos();
             if (alumnos.empty()) { cout << "(!) No hay alumnos registrados." << endl; continue; }

             cout << "\n--- LISTA DE ALUMNOS ---" << endl;
             cout << "ID\t| Nombre\t\t\t| Estado Actual" << endl;
             cout << "----------------------------------------------------------------" << endl;
             for(auto& a : alumnos) {
                 cout << a.id << "\t| " << a.nombre << "\t| ";
                 if (a.id_vinculado == 0) cout << "(SIN TUTOR)" << endl;
                 else cout << "(Tutor: " << db.getNombrePorID(a.id_vinculado) << ")" << endl;
             }
             
             // --- 2. LISTAR TUTORES ---
             vector<Usuario> tutores = db.getAllTutores();
             if (tutores.empty()) { cout << "(!) No hay tutores registrados." << endl; continue; }

             cout << "\n--- LISTA DE TUTORES ---" << endl;
             cout << "ID\t| Nombre\t\t\t| Estado Actual" << endl;
             cout << "----------------------------------------------------------------" << endl;
             for(auto& t : tutores) {
                 cout << t.id << "\t| " << t.nombre << "\t| ";
                 if (t.id_vinculado == 0) cout << "(LIBRE)" << endl;
                 else cout << "(Alumno: " << db.getNombrePorID(t.id_vinculado) << ")" << endl;
             }

             // --- 3. PEDIR DATOS (MODIFICADO PARA 'SALIR') ---
             string entradaID;
             int idA = -1, idT = -1;

             // Bucle para pedir ID Alumno
             while (true) {
                 cout << "\nIntroduce ID del Alumno a asignar (o escribe SALIR): ";
                 cin >> entradaID;
                 
                 if (entradaID == "SALIR" || entradaID == "salir") break; // Rompe el while interno
                 
                 try {
                     idA = stoi(entradaID); // Intentamos convertir texto a numero
                     break; // Si funciona, salimos del bucle de pedir
                 } catch (...) {
                     cout << "(!) Error: Introduce un numero valido o SALIR." << endl;
                 }
             }
             
             if (entradaID == "SALIR" || entradaID == "salir") continue; // Vuelve al menú principal

             // Bucle para pedir ID Tutor
             while (true) {
                 cout << "Introduce ID del Tutor a asignar (o escribe SALIR): ";
                 cin >> entradaID;
                 
                 if (entradaID == "SALIR" || entradaID == "salir") break;
                 
                 try {
                     idT = stoi(entradaID);
                     break;
                 } catch (...) {
                     cout << "(!) Error: Introduce un numero valido o SALIR." << endl;
                 }
             }

             if (entradaID == "SALIR" || entradaID == "salir") continue; // Vuelve al menú principal
             
             // Ejecución final
             if(db.asignarTutorManual(idA, idT)){
                 cout << "\n[EXITO] Asignacion completada correctamente." << endl;
             } else {
                 cout << "\n[ERROR] Algo fallo (revisa si los IDs existen)." << endl;
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
            cout << "--> Estas tutorizando al alumno: " << nombreAlumno << endl;
            cout << "1. Abrir Chat con " << nombreAlumno << endl;
        }

        cout << "2. Cerrar Sesion" << endl;
        cout << "Opcion: ";
        
        if (!(cin >> opcion)) { cin.clear(); cin.ignore(10000, '\n'); continue; }
        
        cin.ignore(); // Limpiar buffer antes de entrar al chat

        if (opcion == 1 && user.id_vinculado != 0) {
             pantallaDeChat(user, user.id_vinculado, db);
        }

    } while (opcion != 2);
}

void mostrarMenuAlumno(Usuario& user, Database& db) {
    int opcion = 0;
    do {
        cout << "\n--- MENU ALUMNO (" << user.nombre << ") ---" << endl;
        
        // Auto-asignación al entrar
        if (user.id_vinculado == 0) {
            if (db.asignarTutorAutomaticamente(user.id)) {
                cout << "INFO! Se te ha asignado tutor automaticamente." << endl;
                user = db.login(user.dni, user.password); // Recargar datos
            } else {
                cout << "(!) Buscando tutor... (No hay disponibles aun)" << endl;
            }
        }
        
        if (user.id_vinculado != 0) {
            string nombreTutor = db.getNombrePorID(user.id_vinculado);
            cout << "--> Tu tutor asignado es: " << nombreTutor << endl;
            cout << "1. Abrir Chat con " << nombreTutor << endl;
        }

        cout << "2. Cerrar Sesion" << endl;
        cout << "Opcion: ";
        
        if (!(cin >> opcion)) { cin.clear(); cin.ignore(10000, '\n'); continue; }
        
        cin.ignore(); // Limpiar buffer antes de entrar al chat

        if (opcion == 1 && user.id_vinculado != 0) {
             pantallaDeChat(user, user.id_vinculado, db);
        }

    } while (opcion != 2);
}

void realizarRegistro(Database& db) {
    string nombre, dni, pass, rol, token;
    int opcionRol;
    
    cout << "\n--- REGISTRO DE NUEVO USUARIO ---" << endl;
    
    cin.ignore(); // Limpiar el Enter del menú anterior
    
    cout << "Nombre completo: ";
    getline(cin, nombre); 
    
    cout << "DNI: "; cin >> dni;
    cout << "Contraseña: "; cin >> pass;
    
    cout << "Selecciona el Rol:\n1. ALUMNO\n2. TUTOR\n3. COORDINADOR\n> ";
    if (!(cin >> opcionRol)) { cin.clear(); cin.ignore(10000, '\n'); return; }
    
    if (opcionRol == 1) {
        rol = "ALUMNO";
    } 
    else if (opcionRol == 2) {
        cout << "Introduce el codigo de autorizacion para TUTORES: ";
        cin >> token;
        if (token == "tutoresUCO") rol = "TUTOR"; 
        else { cout << "Codigo incorrecto." << endl; return; }
    }
    else if (opcionRol == 3) {
        cout << "Introduce el codigo de autorizacion para COORDINADORES: ";
        cin >> token;
        if (token == "coordinadoresUCO") rol = "COORDINADOR"; 
        else { cout << "Codigo incorrecto." << endl; return; }
    } else {
        cout << "Rol no valido." << endl;
        return;
    }

    Usuario nuevo(0, nombre, dni, pass, rol, 0);
    
    if(db.registrarUsuario(nuevo)){
        cout << "¡Registro exitoso! Ya puedes iniciar sesion." << endl;
        if(rol == "ALUMNO") {
             Usuario temp = db.login(dni, pass);
             db.asignarTutorAutomaticamente(temp.id);
        }
    } else {
        cout << "Error: Es posible que el DNI ya exista." << endl;
    }
}

int main() {
    Database db("db/tutorias.db"); 
    int opcionInicio = 0;
    
    while (true) {
        cout << "\n=======================================" << endl;
        cout << "   BIENVENIDO AL GESTOR DE TUTORIAS    " << endl;
        cout << "=======================================" << endl;
        cout << "1. Iniciar Sesion" << endl;
        cout << "2. Registrarse" << endl;
        cout << "3. Salir del programa" << endl;
        cout << "Elige una opcion: ";
        
        if (!(cin >> opcionInicio)) { 
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            cout << "Entrada no valida." << endl; 
            continue; 
        }
        
        if (opcionInicio == 3) break;
        
        if (opcionInicio == 2) {
            realizarRegistro(db);
        }
        else if (opcionInicio == 1) {
            string dni, pass;
            cout << "\n--- INICIO DE SESION ---" << endl;
            cout << "DNI: "; cin >> dni;
            cout << "Contraseña: "; cin >> pass;
            
            Usuario u = db.login(dni, pass);
            
            if (u.id != -1) {
                cout << "Bienvenido, " << u.nombre << endl;
                if (u.rol == "COORDINADOR") mostrarMenuCoordinador(u, db);
                else if (u.rol == "TUTOR") mostrarMenuTutor(u, db);
                else if (u.rol == "ALUMNO") mostrarMenuAlumno(u, db);
            } else {
                cout << "Error: DNI o Contraseña incorrectos." << endl;
            }
        } else {
            cout << "Opcion no valida." << endl;
        }
    }
    return 0;
}
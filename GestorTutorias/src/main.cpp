#include <iostream>
#include <limits> 
#include <vector>
#include <string>
#include <iomanip> 

#include "../include/Database.h"
#include "../include/Usuario.h"

using namespace std;


void limpiarPantalla() {
    // Imprime 50 saltos de línea para "limpiar" la terminal visualmente
    cout << string(50, '\n'); 
}

void pausa() {
    cout << "\n>> Pulsa ENTER para continuar...";
    // Limpieza robusta del buffer para que funcione la pausa
    if (cin.peek() == '\n') cin.ignore();
    cin.get();
}

// Modulos de interfaz visual
void printHeader(string titulo) {
    cout << "\n========================================================" << endl;
    int espacios = (56 - titulo.length()) / 2;
    if (espacios > 0) cout << string(espacios, ' ');
    cout << titulo << endl;
    cout << "========================================================" << endl;
}

void printLinea() {
    cout << "--------------------------------------------------------" << endl;
}

// --- FUNCIÓN AUXILIAR PARA ALERTAS ---
void comprobarAlertas(Usuario& user, Database& db) {
    vector<Alerta> alertas = db.obtenerAlertasPendientes(user.id);
    
    if (!alertas.empty()) {
        limpiarPantalla(); // Limpiamos para que la alerta sea lo único visible
        cout << "\n   /!\\ TIENES " << alertas.size() << " NUEVA(S) ALERTA(S) /!\\   " << endl;
        
        // Interfaz de alertas
        for (auto& a : alertas) {
            cout << "+" << string(54, '-') << "+" << endl;
            cout << "| DE: " << left << setw(48) << a.nombreEmisor << " |" << endl;
            cout << "| FECHA: " << left << setw(45) << a.fecha << " |" << endl;
            cout << "|" << string(54, '-') << "|" << endl;
            cout << "| MENSAJE:                                             |" << endl;
            cout << "| " << left << setw(52) << a.contenido << " |" << endl;
            cout << "+" << string(54, '-') << "+" << endl;
        }
        
        db.marcarAlertasComoLeidas(user.id);
        pausa(); // Esperamos a que el usuario lea antes de borrar
    }
}

// Historial de alertas recibidas
void verHistorialAlertas(Usuario& user, Database& db) {
    limpiarPantalla();
    vector<Alerta> historial = db.getHistorialAlertas(user.id);
    printHeader("HISTORIAL DE ALERTAS RECIBIDAS");
    
    if (historial.empty()) {
        cout << "\n(El historial esta vacio)" << endl;
    } else {
        for(auto& a : historial) {
            cout << " [" << a.fecha << "] De: " << a.nombreEmisor;
            if (a.leida == 0) cout << " (NUEVA)";
            cout << "\n >> " << a.contenido << endl;
            printLinea();
        }
    }
    pausa();
}

// --- FUNCIÓN DE CHAT ---
void pantallaDeChat(Usuario& yo, int idOtroUsuario, Database& db) {

    // Input del chat
    string input;
    // Nombre del tutor o alumno asignado respectivamente
    string nombreOtro = db.getNombrePorID(idOtroUsuario);

    while (true) {
        limpiarPantalla(); // Limpiamos en cada mensaje nuevo para efecto de "refresco"
        printHeader("CHAT CON: " + nombreOtro);

        vector<Mensaje> historial = db.getHistorialChat(yo.id, idOtroUsuario);
        
        cout << endl;
        if (historial.empty()) {
            cout << "       (No hay mensajes previos. Saluda!)" << endl;
        } else {
            for (Mensaje m : historial) {
                if (m.nombreEmisor == yo.nombre) {
                    cout << right << setw(55) << ("(Yo) " + m.contenido) << endl;
                    cout << right << setw(55) << ("[" + m.fecha + "]") << endl;
                } else {
                    cout << left <<  m.nombreEmisor << ": " << m.contenido << endl;
                    cout << left << " [" << m.fecha << "]" << endl;
                }
                cout << endl; 
            }
        }

        printLinea();
        cout << "[Escribe mensaje] o [SALIR]: ";

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
        // Limpiamos menu al entrar
        limpiarPantalla(); 
        // Interfaz
        printHeader("PANEL DE COORDINACION: " + user.nombre);
        cout << " 1. Asignar Tutor manualmente" << endl;
        cout << " 2. Ver Metricas (Tutor o Alumno)" << endl;
        cout << " 3. Cerrar Sesion" << endl;
        printLinea();
        cout << " > Elige opcion: ";
        
        if (!(cin >> opcion)) { cin.clear(); cin.ignore(10000, '\n'); continue; }

        if (opcion == 1) {
             limpiarPantalla();
             printHeader("ASIGNACION MANUAL");

             // Listar Alumnos
             vector<Usuario> alumnos = db.getAllAlumnos();
             if (alumnos.empty()) { cout << "(!) No hay alumnos registrados." << endl; pausa(); continue; }

             cout << left << setw(5) << "ID" << "| " << setw(25) << "NOMBRE ALUMNO" << "| ESTADO" << endl;
             printLinea();
             for(auto& a : alumnos) {
                 string estado = (a.id_vinculado == 0) ? "SIN TUTOR" : ("Con: " + db.getNombrePorID(a.id_vinculado));
                 cout << left << setw(5) << a.id << "| " << setw(25) << a.nombre << "| " << estado << endl;
             }
             cout << endl; 

             // Listar Tutores
             vector<Usuario> tutores = db.getAllTutores();
             if (tutores.empty()) { cout << "(!) No hay tutores registrados." << endl; pausa(); continue; }

             cout << left << setw(5) << "ID" << "| " << setw(25) << "NOMBRE TUTOR" << "| ESTADO" << endl;
             printLinea();
             for(auto& t : tutores) {
                 string estado = (t.id_vinculado == 0) ? "LIBRE" : ("Ocupado (" + db.getNombrePorID(t.id_vinculado) + ")");
                 cout << left << setw(5) << t.id << "| " << setw(25) << t.nombre << "| " << estado << endl;
             }

             // Pedir IDs
             string entradaID;
             int idA = -1, idT = -1;

             cout << "\n[Escribe 'SALIR' para cancelar]" << endl;
             
             while (true) {
                 cout << ">> Introduce ID del ALUMNO: "; cin >> entradaID;
                 if (entradaID == "SALIR" || entradaID == "salir") break;
                 try { idA = stoi(entradaID); break; } catch (...) { cout << "Numero invalido." << endl; }
             }
             if (entradaID == "SALIR" || entradaID == "salir") continue;

             while (true) {
                 cout << ">> Introduce ID del TUTOR: "; cin >> entradaID;
                 if (entradaID == "SALIR" || entradaID == "salir") break;
                 try { idT = stoi(entradaID); break; } catch (...) { cout << "Numero invalido." << endl; }
             }
             if (entradaID == "SALIR" || entradaID == "salir") continue;
             
             if(db.asignarTutorManual(idA, idT)) cout << "\n[OK] Asignacion completada con exito." << endl;
             else cout << "\n[ERROR] Fallo en la operacion. Revisa los IDs." << endl;
             
             pausa(); // Pausa para leer el resultado antes de limpiar
        }
        else if (opcion == 2) {
            // Limpiamos antes de entrar al menu
            limpiarPantalla();
            printHeader("CONSULTA DE METRICAS");
            
            int subopcion;
            cout << " 1. Ver metricas de un TUTOR\n 2. Ver metricas de un ALUMNO\n > ";
            if(cin >> subopcion) {
                vector<Usuario> lista;
                if(subopcion == 1) lista = db.getAllTutores();
                else if(subopcion == 2) lista = db.getAllAlumnos();
                else { cout << "Opcion no valida." << endl; pausa(); continue; }

                cout << endl;
                cout << left << setw(5) << "ID" << "| " << "NOMBRE" << endl;
                printLinea();
                for(auto& u : lista) cout << left << setw(5) << u.id << "| " << u.nombre << endl;
                printLinea();

                int idSel;
                cout << ">> Introduce ID para ver detalles: ";
                if(cin >> idSel) {
                    string nombreSel = db.getNombrePorID(idSel);
                    if(nombreSel == "Desconocido") cout << "Usuario no encontrado." << endl;
                    else {
                        limpiarPantalla();
                        printHeader("METRICAS: " + nombreSel);
                        cout << " + Alertas Enviadas:   " << db.getNumAlertasEnviadas(idSel) << endl;
                        cout << " + Alertas Recibidas:  " << db.getNumAlertasRecibidas(idSel) << endl;
                        
                        bool activo = db.tieneChatActivo(idSel);
                        cout << " + Estado del Chat:    " << (activo ? "[ ACTIVO ]" : "[ INACTIVO ]") << endl;
                        printLinea();
                        pausa();
                    }
                } else { cin.clear(); cin.ignore(10000, '\n'); }
                // Agregamos lineas para limpiar los menus anteriores
            } else { cin.clear(); cin.ignore(10000, '\n'); }
        }

    } while (opcion != 3);
}

// Menu del tutor
void mostrarMenuTutor(Usuario& user, Database& db) {
    int opcion = 0;
    do {
        comprobarAlertas(user, db); 
        
        limpiarPantalla(); 
        // Limpiamos pantalla antes de entrar al menu
        printHeader("AREA DEL TUTOR: " + user.nombre);
        
        if (user.id_vinculado == 0) {
            cout << " [!] ESTADO: No tienes alumno asignado." << endl;
            cout << "     (Espera a que te asigne el coordinador)" << endl;
        } else {
            string nombreAlumno = db.getNombrePorID(user.id_vinculado);
            cout << " [OK] ALUMNO ASIGNADO: " << nombreAlumno << endl;
            printLinea();
            cout << " 1. Abrir Chat con " << nombreAlumno << endl;
            cout << " 2. Enviar ALERTA (Aviso Urgente)" << endl;
            cout << " 3. Cerrar/Reiniciar Chat" << endl;
        }
        cout << " 4. Ver Historial de Alertas Recibidas" << endl;
        cout << " 5. Cerrar Sesion" << endl;
        printLinea();
        cout << " > Elige opcion: ";
        
        if (!(cin >> opcion)) { cin.clear(); cin.ignore(10000, '\n'); continue; }

        if (user.id_vinculado != 0) {
            if (opcion == 1) {
                pantallaDeChat(user, user.id_vinculado, db);
            } 
            else if (opcion == 2) {
                string contenido;
                cout << "\n>> Escribe la ALERTA para el alumno: ";
                if(cin.peek()=='\n') cin.ignore();
                getline(cin, contenido);
                if(!contenido.empty() && db.enviarAlerta(user.id, user.id_vinculado, contenido)) {
                    cout << "   [OK] Alerta enviada correctamente." << endl;
                    pausa();
                }
            }
            else if (opcion == 3) {
                char confirm;
                cout << "\n[PELIGRO] ¿Borrar todo el historial de chat? (s/n): "; cin >> confirm;
                if(confirm == 's' || confirm == 'S') {
                    if(db.borrarChat(user.id, user.id_vinculado)) {
                        cout << "   [OK] Chat reiniciado." << endl;
                        pausa();
                    }
                }
            }
        }
        if (opcion == 4) verHistorialAlertas(user, db);

    } while (opcion != 5);
}

// Menu de alumno
void mostrarMenuAlumno(Usuario& user, Database& db) {
    int opcion = 0;
    do {
        if (user.id_vinculado == 0) {
            // Asignacion automatica si no tiene tutor al iniciar sesion
            if (db.asignarTutorAutomaticamente(user.id)) user = db.login(user.dni, user.password);
        }
        
        comprobarAlertas(user, db);

        // Limpiamos antes de entrar al menu
        limpiarPantalla();
        printHeader("AREA DEL ALUMNO: " + user.nombre);
        
        if (user.id_vinculado != 0) {
            string nombreTutor = db.getNombrePorID(user.id_vinculado);
            cout << " [OK] TUTOR ASIGNADO: " << nombreTutor << endl;
            printLinea();
            cout << " 1. Abrir Chat con " << nombreTutor << endl;
            cout << " 2. Enviar ALERTA (Aviso Urgente)" << endl;
        } else {
            cout << " [!] ESTADO: Buscando tutor disponible..." << endl;
        }
        cout << " 3. Ver Historial de Alertas Recibidas" << endl;
        cout << " 4. Cerrar Sesion" << endl;
        printLinea();
        cout << " > Elige opcion: ";
        
        if (!(cin >> opcion)) { cin.clear(); cin.ignore(10000, '\n'); continue; }

        if (user.id_vinculado != 0) {
            if (opcion == 1) pantallaDeChat(user, user.id_vinculado, db);
            else if (opcion == 2) {
                string contenido;
                cout << "\n>> Escribe la ALERTA para el tutor: ";
                if(cin.peek()=='\n') cin.ignore();
                getline(cin, contenido);
                if(!contenido.empty() && db.enviarAlerta(user.id, user.id_vinculado, contenido)) {
                    cout << "   [OK] Alerta enviada correctamente." << endl;
                    pausa();
                }
            }
        }
        if (opcion == 3) verHistorialAlertas(user, db);

    } while (opcion != 4);
}

void realizarRegistro(Database& db) {
    limpiarPantalla();
    string nombre, dni, pass, rol, token;
    int opcionRol;
    
    printHeader("REGISTRO DE NUEVO USUARIO");
    
    if (cin.peek() == '\n') cin.ignore(); 
    
    cout << " > Nombre completo: "; getline(cin, nombre); 
    cout << " > DNI: "; cin >> dni;
    cout << " > Contraseña: "; cin >> pass;
    
    cout << "\n--- Selecciona tu Rol ---" << endl;
    cout << " 1. ALUMNO" << endl;
    cout << " 2. TUTOR" << endl;
    cout << " 3. COORDINADOR" << endl;
    cout << " > Opcion: ";
    if (!(cin >> opcionRol)) { cin.clear(); cin.ignore(10000, '\n'); return; }
    
    if (opcionRol == 1) {
        rol = "ALUMNO";
    } 
    else if (opcionRol == 2) {
        cout << " > Token de seguridad (TUTOR): "; cin >> token;
        if (token == "tutoresUCO") rol = "TUTOR"; 
        else { cout << " [X] Token incorrecto." << endl; pausa(); return; }
    }
    else if (opcionRol == 3) {
        cout << " > Token de seguridad (COORD): "; cin >> token;
        if (token == "coordinadoresUCO") rol = "COORDINADOR"; 
        else { cout << " [X] Token incorrecto." << endl; pausa(); return; }
    } else {
        cout << " [X] Rol no valido." << endl;
        pausa();
        return;
    }

    Usuario nuevo(0, nombre, dni, pass, rol, 0);
    
    if(db.registrarUsuario(nuevo)){
        cout << "\n [OK] ¡Registro exitoso! Ya puedes iniciar sesion." << endl;
        if(rol == "ALUMNO") {
             Usuario temp = db.login(dni, pass);
             db.asignarTutorAutomaticamente(temp.id);
        }
    } else {
        cout << "\n [X] Error: Es posible que el DNI ya exista." << endl;
    }
    pausa();
}

int main() {
    Database db("db/tutorias.db"); 
    int opcionInicio = 0;
    
    while (true) {
        limpiarPantalla(); 
        // MENÚ PRINCIPAL
        cout << "\n========================================================" << endl;
        cout << "          SISTEMA DE GESTION DE TUTORIAS UCO            " << endl;
        cout << "========================================================" << endl;
        cout << " 1. Iniciar Sesion" << endl;
        cout << " 2. Registrarse" << endl;
        cout << " 3. Salir del programa" << endl;
        printLinea();
        cout << " > Elige una opcion: ";
        
        if (!(cin >> opcionInicio)) { 
            cin.clear(); 
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            cout << " [X] Entrada no valida." << endl; 
            continue; 
        }
        
        if (opcionInicio == 3) {
            cout << "\nCerrando sistema... Hasta luego." << endl;
            break;
        }
        
        if (opcionInicio == 2) {
            realizarRegistro(db);
        }
        else if (opcionInicio == 1) {
            string dni, pass;
            cout << "\n--- INICIO DE SESION ---" << endl;
            cout << " > DNI: "; cin >> dni;
            cout << " > Contraseña: "; cin >> pass;
            
            Usuario u = db.login(dni, pass);
            
            if (u.id != -1) {
                cout << "\n [OK] Acceso correcto. Bienvenido " << u.nombre << "." << endl;
                pausa(); // Pausa breve para ver que el login fue bien
                if (u.rol == "COORDINADOR") mostrarMenuCoordinador(u, db);
                else if (u.rol == "TUTOR") mostrarMenuTutor(u, db);
                else if (u.rol == "ALUMNO") mostrarMenuAlumno(u, db);
            } else {
                cout << "\n [X] Error: DNI o Contraseña incorrectos." << endl;
                pausa();
            }
        } else {
            cout << " [X] Opcion no valida." << endl;
            pausa();
        }
    }
    return 0;
}
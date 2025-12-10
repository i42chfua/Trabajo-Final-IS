#include "gtest/gtest.h"
#include <cstdio> 

#include "Database.h"
#include "Usuario.h"

// Borrar la BD antes de cada test
class TestTutorias : public ::testing::Test {
protected:
    void SetUp() override {
        std::remove("test_tutorias.db");
    }
};

// PRUEBA 1: Registro y Duplicados
// Objetivo: Verificar que no se pueden registrar dos usuarios con el mismo DNI.
TEST_F(TestTutorias, RegistroDuplicado) {
    Database db("test_tutorias.db");

    // Crear usuarios
    Usuario u1(0, "Pepe", "111A", "pass", "ALUMNO", 0);
    Usuario u2(0, "Juan", "111A", "pass", "TUTOR", 0); // Mismo DNI

    // El primero debe entrar (TRUE)
    EXPECT_TRUE(db.registrarUsuario(u1));

    // El segundo debe fallar (FALSE)
    EXPECT_FALSE(db.registrarUsuario(u2));
}

// PRUEBA 2: Login Incorrecto
// Objetivo: Comprobar que el sistema rechaza contraseñas erróneas.
TEST_F(TestTutorias, LoginIncorrecto) {
    Database db("test_tutorias.db");

    // Registramos un usuario válido
    db.registrarUsuario(Usuario(0, "Ana", "222B", "secreto", "ALUMNO", 0));

    // Intentamos login con contraseña mal
    Usuario resultado = db.login("222B", "incorrecta");

    // Debe devolver ID -1 (Usuario vacío/error)
    EXPECT_EQ(resultado.id, -1);
}

// Prueba 3: Asignación Automática
TEST_F(TestTutorias, AsignacionAutomatica) {
    Database db("test_tutorias.db");

    // Crear usuarios
    db.registrarUsuario(Usuario(0, "Profe", "T1", "p", "TUTOR", 0));
    db.registrarUsuario(Usuario(0, "Alumno", "A1", "p", "ALUMNO", 0));

    // IDs reales
    int idT = db.login("T1", "p").id;
    int idA = db.login("A1", "p").id;

    // Ejecutar lógica
    EXPECT_TRUE(db.asignarTutorAutomaticamente(idA));

    // Verificar
    Usuario alumno = db.login("A1", "p");
    EXPECT_EQ(alumno.id_vinculado, idT);
}

// Prueba 4: Reasignación Manual (Compleja)
TEST_F(TestTutorias, ReasignacionManual) {
    Database db("test_tutorias.db");

    // 1 Alumno, 2 Tutores
    db.registrarUsuario(Usuario(0, "A", "A", "p", "ALUMNO", 0));
    db.registrarUsuario(Usuario(0, "T1", "T1", "p", "TUTOR", 0));
    db.registrarUsuario(Usuario(0, "T2", "T2", "p", "TUTOR", 0));

    int idA = db.login("A", "p").id;
    int idT1 = db.login("T1", "p").id;
    int idT2 = db.login("T2", "p").id;

    // Asignar a T1
    db.asignarTutorManual(idA, idT1);

    // Cambiar a T2
    EXPECT_TRUE(db.asignarTutorManual(idA, idT2));

    // Verificar: Alumno tiene a T2, T1 está libre (0)
    Usuario alumno = db.login("A", "p");
    Usuario tutor1 = db.login("T1", "p");

    EXPECT_EQ(alumno.id_vinculado, idT2);
    EXPECT_EQ(tutor1.id_vinculado, 0);
}

// Prueba 5: Chat
TEST_F(TestTutorias, HistorialChat) {
    Database db("test_tutorias.db");

    db.registrarUsuario(Usuario(0, "U1", "1", "p", "TUTOR", 0));
    db.registrarUsuario(Usuario(0, "U2", "2", "p", "ALUMNO", 0));
    int id1 = db.login("1", "p").id;
    int id2 = db.login("2", "p").id;

    db.enviarMensaje(id1, id2, "Mensaje de Prueba GoogleTest");

    vector<Mensaje> historial = db.getHistorialChat(id1, id2);

    ASSERT_FALSE(historial.empty());
    EXPECT_EQ(historial[0].contenido, "Mensaje de Prueba GoogleTest");
}

// Prueba 6: Alertas 
TEST_F(TestTutorias, FlujoAlertas) {
    Database db("test_tutorias.db");

    // 1. Crear usuarios reales para que existan en la BD
    db.registrarUsuario(Usuario(0, "EmisorTest", "E1", "p", "TUTOR", 0));
    db.registrarUsuario(Usuario(0, "ReceptorTest", "R1", "p", "ALUMNO", 0));

    // 2. Obtener sus IDs autogenerados por SQLite
    int idEmisor = db.login("E1", "p").id;
    int idReceptor = db.login("R1", "p").id;

    // 3. Enviar Alerta
    db.enviarAlerta(idEmisor, idReceptor, "Alerta GTest");

    // 4. Verificar pendiente
    vector<Alerta> pendientes = db.obtenerAlertasPendientes(idReceptor);
    ASSERT_FALSE(pendientes.empty()); // Usamos ASSERT para no seguir si falla
    EXPECT_EQ(pendientes.size(), 1);
    EXPECT_EQ(pendientes[0].contenido, "Alerta GTest");

    // 5. Marcar leída
    db.marcarAlertasComoLeidas(idReceptor);

    // 6. Verificar vacía
    pendientes = db.obtenerAlertasPendientes(idReceptor);
    EXPECT_EQ(pendientes.size(), 0);
}
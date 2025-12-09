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
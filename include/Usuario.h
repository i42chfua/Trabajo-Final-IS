#ifndef USUARIO_H
#define USUARIO_H

#include <string>
#include <iostream>

using namespace std;

// Clase para mover datos por el programa
class Usuario {
public:
    int id;
    string nombre;
    string dni;
    string rol; // "ALUMNO", "TUTOR", "COORDINADOR"
    int id_vinculado; // Si es Alumno -> ID del Tutor. Si es Tutor -> ID del Alumno.

    // Constructor vacio
    Usuario() : id(-1), id_vinculado(0) {}

    // Constructor completo
    Usuario(int _id, string _nombre, string _dni, string _rol, int _id_vinculado) 
        : id(_id), nombre(_nombre), dni(_dni), rol(_rol), id_vinculado(_id_vinculado) {}
};

#endif

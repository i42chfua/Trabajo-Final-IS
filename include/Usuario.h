#ifndef USUARIO_H
#define USUARIO_H

#include <string>
#include <iostream>

using namespace std;

class Usuario {

    public:

        int id;
        string nombre;
        string dni;
        string password; 
        string rol;      
        int id_vinculado; 

        // Constructor inicial
        Usuario() {
            id = -1;
            id_vinculado = 0;
        }

        // Constructor 
        Usuario(int _id, string _nombre, string _dni, string _pass, string _rol, int _id_vinculado) {
            id = _id;
            nombre = _nombre;
            dni = _dni;
            password = _pass; 
            rol = _rol;
            id_vinculado = _id_vinculado;
        }
};

#endif

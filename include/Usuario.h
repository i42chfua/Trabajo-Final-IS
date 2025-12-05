#ifndef USUARIO_H
#define USUARIO_H

#include <string>
#include <iostream>

class Usuario {

    public:

        int id;
        std::string nombre;
        std::string dni;
        std::string password; 
        std::string rol;      
        int id_vinculado; 

        // Constructor inicial
        Usuario() {
            id = -1;
            id_vinculado = 0;
        }

        // Constructor 
        Usuario(int _id, std::string _nombre, std::string _dni, std::string _pass, std::string _rol, int _id_vinculado) {
            id = _id;
            nombre = _nombre;
            dni = _dni;
            password = _pass; 
            rol = _rol;
            id_vinculado = _id_vinculado;
        }
};

#endif

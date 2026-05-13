#include <string>
#include <iostream>
using namespace std;
#include <cstdlib>
#include <vector>
#include <fstream>
#include <cstring>
#include "compara_ascii.cpp"

struct Nodo {
    unsigned char* clave; 
    Nodo* sig;            
    Nodo* ant;            
    Nodo* inf;            
     
    Nodo(const unsigned char* palabra) {
        int largo = strlen((const char*)palabra);
        clave = new unsigned char[largo + 1];
        strcpy((char*)clave, (const char*)palabra);
        sig = ant = inf = nullptr;
    }
 };

///////////////////////////////////////7

 void imprimir_lista(Nodo* cabeza) {
    Nodo* actual = cabeza;
    int contador = 1;
    
    cout << "--- Contenido de la Lista Base (L1) ---" << endl;
    while (actual != nullptr) {
        // Casteamos a char* para que cout sepa que es una palabra
        cout << "Nodo " << contador << ": " << (char*)actual->clave << endl;
        
        // El paso clave: saltar al siguiente nodo
        actual = actual->sig;
        contador++;
    }
    cout << "---------------------------------------" << endl;
}
//////////////////////////////////////////////////////////////////7

void enlaza_listas(Nodo* nodoA,Nodo* nodoB){
    if (nodoA) nodoA->sig= nodoB;
    if (nodoB) nodoB->ant= nodoA;

}

void Enlace_Sub_Lista(Nodo* nodo,Nodo* subnodo){
    if (nodo) nodo->inf=subnodo;

}

Nodo* guarda_clave(const unsigned char* palabra, Nodo* ultimo){
    Nodo* nuevo = new Nodo(palabra);

    if (ultimo!= nullptr){
        enlaza_listas(ultimo,nuevo);
    }

    return nuevo;


}

Nodo* Crea_Clave(string texto){
    ifstream archivo(texto);
    string lineas;

    Nodo* cabeza = nullptr; 
    Nodo* ultimo = nullptr;

    while(getline(archivo,lineas)){
        if (lineas.empty()) continue;

        const unsigned char* palabra = (const unsigned char*)lineas.c_str();

        Nodo* nuevo = guarda_clave(palabra,ultimo);

        if (cabeza==nullptr){
            cabeza=nuevo;
        }
        ultimo=nuevo;
    }
    archivo.close();
    return cabeza;

}


int main(int argc, char* argv[]){
    string nombre_archivo = argv[1];

    Nodo* prueba_1=Crea_Clave(nombre_archivo);

    if (prueba_1 != nullptr) {
        imprimir_lista(prueba_1);
    } else {
        cout << "La lista esta vacia o el archivo no existe." << endl;
    }

    return 0;



}


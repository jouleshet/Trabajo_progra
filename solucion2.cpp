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

Nodo* Enlace_Sub_Lista(Nodo* nodo,Nodo* subnodo){
    if (nodo) nodo->inf=subnodo;

}

Nodo* insertarOrdenado(Nodo* cabeza, const unsigned char* palabra) {
    Nodo* nuevo = new Nodo(palabra);

    if (cabeza == nullptr || compararLexicografico(palabra,cabeza->clave) < 0) {
        nuevo->sig = cabeza;
        if (cabeza != nullptr) cabeza->ant = nuevo;
        return nuevo; // El nuevo es la nueva cabeza
    }

    // Caso B: Buscar el lugar en medio o al final
    Nodo* actual = cabeza;
    while (actual->sig != nullptr && compararLexicografico(palabra,actual->sig->clave) > 0) {
        actual = actual->sig;
    }

    // Insertar después de 'actual'
    nuevo->sig = actual->sig;
    nuevo->ant = actual;
    if (actual->sig != nullptr) (actual->sig)->ant = nuevo;
    actual->sig = nuevo;
    return cabeza;
}

Nodo* Crea_Clave(string texto){
    ifstream archivo(texto);
    string lineas;

    Nodo* cabeza = nullptr; 

    while(getline(archivo,lineas)){
        if (lineas.empty()) continue;

        const unsigned char* palabra = (const unsigned char*)lineas.c_str();

        cabeza=insertarOrdenado(cabeza,palabra);
        
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


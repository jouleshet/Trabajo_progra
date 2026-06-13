#include <string>
#include <iostream>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <cstring>
#include <chrono>
#include "compara_ascii.cpp"
#include <algorithm>
#include <random>

struct NodoK {
    unsigned char** claves; 
    NodoK** hijos;      
    int cantidad_claves;   

    NodoK(int k) {
        claves = new unsigned char*[k];
        hijos = new NodoK*[k + 1];
        cantidad_claves = 0;


        // lineas para iniciar todo en nulo, y no quede basura
        for (int i = 0; i < k; i++) claves[i] = nullptr;
        for (int i = 0; i <= k; i++) hijos[i] = nullptr;
    }
};


void insertarEnNodoEstatico(NodoK* nodo, const unsigned char* palabra, int k) {
    int i = nodo->cantidad_claves - 1;
   
    int largo = strlen((const char*)palabra);
    unsigned char* nuevaClave = new unsigned char[largo + 1];
    strcpy((char*)nuevaClave, (const char*)palabra);

    while (i >= 0 && compararLexicografico(nuevaClave, nodo->claves[i]) < 0) {
        nodo->claves[i + 1] = nodo->claves[i];
        nodo->hijos[i + 2] = nodo->hijos[i + 1];
        i--;
    }

    nodo->claves[i + 1] = nuevaClave;
    nodo->cantidad_claves++;
}

NodoK* insertarKario(NodoK* raiz, const unsigned char* palabra, int k) {
    if (raiz == nullptr) {
        NodoK* nuevoNodo = new NodoK(k);
        insertarEnNodoEstatico(nuevoNodo, palabra, k);
        return nuevoNodo;
    }

    int i = 0;
    while (i < raiz->cantidad_claves && compararLexicografico(palabra, raiz->claves[i]) > 0) {
        i++;
    }

    if (i < raiz->cantidad_claves && compararLexicografico(palabra, raiz->claves[i]) == 0) {
        return raiz;
    }

    bool esHoja = true;
    for (int j = 0; j <= k; j++) {
        if (raiz->hijos[j] != nullptr) {
            esHoja = false;
            break;
        }
    }

    if (esHoja && raiz->cantidad_claves < k) {
        insertarEnNodoEstatico(raiz, palabra, k);
        return raiz;
    }

    raiz->hijos[i] = insertarKario(raiz->hijos[i], palabra, k);
    return raiz;
}

bool buscarKario(NodoK* raiz, const unsigned char* palabraBuscada) {
    if (raiz == nullptr) return false;

    int i = 0;
    while (i < raiz->cantidad_claves && compararLexicografico(palabraBuscada, raiz->claves[i]) > 0) {
        i++;
    }

    if (i < raiz->cantidad_claves && compararLexicografico(palabraBuscada, raiz->claves[i]) == 0) {
        return true;
    }

    return buscarKario(raiz->hijos[i], palabraBuscada); 
}


int main(int argc,char* argv[]){

    string ArchivoD1=argv[1];

    int k= stoi(argv[2]);

    NodoK* raiz = nullptr;
    ifstream fileD1(ArchivoD1);
    string linea;
    vector<string> palabrasCargadas;

    while (getline(fileD1, linea)) {
        if (!linea.empty()) {
            raiz = insertarKario(raiz, (const unsigned char*)linea.c_str(), k);
            palabrasCargadas.push_back(linea); // es un vector que utilizaremos en los experimentos, no mover.
        }
    }
    fileD1.close();

    
}

/*QUE HACER
   CREAR ESTRUCTURA QUE GUARDA PALABRAS QUE FUNCIONE SIMILAR A UN ARREGLO , EL CUAL TIENE QUE ESTAR SIEMPRE ORDENADO
   EL ARREGLO TIENE CAPACIDAD MAXIMA, SI SE LLENA CREAR OTRO, QUE TENGA MAYOR CAPACIDAD, PERO QUE EL AUMENTO SEA
   DECENTE, OSEA, NO AUMENTARLO EN 1 PQ SINO CADA PALABRA HARIA OTRO MAS esto es el OVERHEAD

   usar un INDICE que guarde las posiciones de las letras, asi al buscar un elemento solo uso el valor del indice de la 1ra letra y la 
*/

#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <cstring>
#include <chrono>
#include <algorithm>
#include <random>

using namespace std;

int indice[26]; 
const int CAPACIDAD_INICIAL = 100; 

int compararLexicografico(const char* str1, const char* str2) {
    return strcmp(str1, str2);
}

struct ArregloPalabras { 
    unsigned char** palabras; 
    int capacidad; 
    int cantidad; 
};

void crearArreglo(ArregloPalabras& arreglo, int capacidadInicial) {
    arreglo.palabras = new unsigned char*[capacidadInicial]; 
    arreglo.capacidad = capacidadInicial; 
    arreglo.cantidad = 0; 
}

void destruirArreglo(ArregloPalabras& arreglo) {
    for (int i = 0; i < arreglo.cantidad; i++) {
        delete[] arreglo.palabras[i];
    }
    delete[] arreglo.palabras;
}

void aumentarCapacidad(ArregloPalabras& arreglo) {
    int nuevaCapacidad = arreglo.capacidad * 2; 
    unsigned char** nuevoArreglo = new unsigned char*[nuevaCapacidad]; 

    for (int i = 0; i < arreglo.cantidad; i++) {
        nuevoArreglo[i] = arreglo.palabras[i];
    }

    delete[] arreglo.palabras; 
    arreglo.palabras = nuevoArreglo; 
    arreglo.capacidad = nuevaCapacidad; 
}

int busqueda_binaria(ArregloPalabras& arreglo, unsigned char* palabra, int* indice) {
    int letra = palabra[0] - 'a';
    int inicio = 0;
    int fin = arreglo.cantidad - 1; 

    if (letra >= 0 && letra < 26 && indice[letra] != -1) {
        inicio = indice[letra]; 
    } else {
        return -1;
    }
   
    for (int i = letra + 1; i < 26; i++) {
        if (indice[i] != -1) {
            fin = indice[i] - 1; 
            break;     
        }
    }
      
    while (inicio <= fin) {
        int medio = inicio + (fin - inicio) / 2;
        int comparacion = compararLexicografico((char*)arreglo.palabras[medio], (char*)palabra);

        if (comparacion == 0) return medio;
        else if (comparacion < 0) inicio = medio + 1;
        else fin = medio - 1;
    }
    return -1; 
}

void anadirPalabra(ArregloPalabras& arreglo, unsigned char* palabra, int* indice) {
    if (arreglo.cantidad >= arreglo.capacidad) aumentarCapacidad(arreglo); 
    
    int posicion = arreglo.cantidad; 
    int letra = palabra[0] - 'a'; 

    if (letra >= 0 && letra < 26 && indice[letra] != -1) { 
        posicion = indice[letra];
    } else if (letra >= 0 && letra < 26) {
        for (int i = letra + 1; i < 26; i++) {
            if (indice[i] != -1) {
                posicion = indice[i]; 
                break;
            }
        }
    }

    while (posicion < arreglo.cantidad && strcmp((char*)arreglo.palabras[posicion], (char*)palabra) < 0) {
        posicion++; 
    }

    for (int i = arreglo.cantidad; i > posicion; i--) {
        arreglo.palabras[i] = arreglo.palabras[i - 1];
    }

    arreglo.palabras[posicion] = palabra; 
    arreglo.cantidad++;

    if (letra >= 0 && letra < 26) {
        if (indice[letra] == -1 || posicion < indice[letra]) indice[letra] = posicion; 
        for (int i = letra + 1; i < 26; i++) {
            if (indice[i] != -1) indice[i]++; 
        }
    }
}

void eliminarPalabra(ArregloPalabras& arreglo, unsigned char* palabra, int* indice) {
    int posicion = busqueda_binaria(arreglo, palabra, indice);
    if (posicion == -1) return; 

    int letra = palabra[0] - 'a';
    if (letra >= 0 && letra < 26 && indice[letra] == posicion) {
        if (posicion + 1 < arreglo.cantidad && arreglo.palabras[posicion + 1][0] - 'a' == letra) {
            indice[letra] = posicion; 
        } else {
            indice[letra] = -1; 
        }
    }

    delete[] arreglo.palabras[posicion];

    for (int i = posicion; i < arreglo.cantidad - 1; i++) {
        arreglo.palabras[i] = arreglo.palabras[i + 1];
    }
    arreglo.cantidad--;

    if (letra >= 0 && letra < 26) {
        for (int i = letra + 1; i < 26; i++) {
            if (indice[i] != -1) indice[i]--;
        }
    }
}

int main() {
    for (int i = 0; i < 26; i++) indice[i] = -1;
    ArregloPalabras miArreglo;
    crearArreglo(miArreglo, CAPACIDAD_INICIAL);

    // --- EXPERIMENTO 1: Construcción ---
    cout << "1. Construyendo arreglo con D1.txt..." << endl;
    ifstream archivoD1("D1.txt");
    auto inicioConst = chrono::high_resolution_clock::now();
    
    char temp[100]; 
    while (archivoD1.getline(temp, 100)) {
        if (strlen(temp) == 0) continue; 
        unsigned char* palabra = new unsigned char[strlen(temp) + 1];
        strcpy((char*)palabra, temp);
        anadirPalabra(miArreglo, palabra, indice);
    }
    archivoD1.close();
    
    auto finConst = chrono::high_resolution_clock::now();
    cout << "Tiempo de construccion: " << chrono::duration<double>(finConst - inicioConst).count() << " segundos.\n" << endl;

    // --- Cargar D2 para los siguientes experimentos ---
    vector<string> palabrasD2;
    ifstream archivoD2("D2.txt");
    string linea;
    while(getline(archivoD2, linea)) {
        if(!linea.empty()) palabrasD2.push_back(linea);
    }
    archivoD2.close();

    random_device rd;
    mt19937 g(rd());

    // --- EXPERIMENTO 2: Búsquedas ---
    cout << "2. Buscando 10.000 palabras aleatorias..." << endl;
    vector<string> palabrasBuscar = palabrasD2;
    shuffle(palabrasBuscar.begin(), palabrasBuscar.end(), g);
    int num_busquedas = min(10000, (int)palabrasBuscar.size());
    
    auto inicioBusq = chrono::high_resolution_clock::now();
    for (int i = 0; i < num_busquedas; i++) {
        busqueda_binaria(miArreglo, (unsigned char*)palabrasBuscar[i].c_str(), indice);
    }
    auto finBusq = chrono::high_resolution_clock::now();
    cout << "Tiempo total de busqueda: " << chrono::duration<double>(finBusq - inicioBusq).count() << " segundos.\n" << endl;

    // --- EXPERIMENTO 3: Inserciones ---
    cout << "3. Insertando 5.000 palabras aleatorias..." << endl;
    int limiteIns = min(5000, (int)palabrasD2.size());
    vector<string> palabrasInsertar(palabrasD2.begin(), palabrasD2.begin() + limiteIns);
    shuffle(palabrasInsertar.begin(), palabrasInsertar.end(), g);

    auto inicioIns = chrono::high_resolution_clock::now();
    for(const string& p : palabrasInsertar) {
        unsigned char* palabraNueva = new unsigned char[p.length() + 1];
        strcpy((char*)palabraNueva, p.c_str());
        anadirPalabra(miArreglo, palabraNueva, indice);
    }
    auto finIns = chrono::high_resolution_clock::now();
    cout << "Tiempo total de insercion: " << chrono::duration<double>(finIns - inicioIns).count() << " segundos.\n" << endl;

    // --- EXPERIMENTO 4: Eliminaciones ---
    cout << "4. Eliminando 5.000 palabras aleatorias..." << endl;
    vector<string> palabrasEliminar;
    if (palabrasD2.size() > 5000) palabrasEliminar.assign(palabrasD2.end() - 5000, palabrasD2.end());
    else palabrasEliminar = palabrasD2;
    shuffle(palabrasEliminar.begin(), palabrasEliminar.end(), g);

    auto inicioDel = chrono::high_resolution_clock::now();
    for(const string& p : palabrasEliminar) {
        eliminarPalabra(miArreglo, (unsigned char*)p.c_str(), indice);
    }
    auto finDel = chrono::high_resolution_clock::now();
    cout << "Tiempo total de eliminacion: " << chrono::duration<double>(finDel - inicioDel).count() << " segundos.\n" << endl;

    destruirArreglo(miArreglo);
    return 0;
}

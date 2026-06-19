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

const int CAPACIDAD_INICIAL = 100; 

struct ArregloPalabras { 
    unsigned char** palabras; 
    int capacity; 
    int cantidad; 
};

int compararLexicografico(const char* str1, const char* str2) {
    return strcmp(str1, str2);
}

void crearArreglo(ArregloPalabras& arreglo, int capacidadInicial) {
    arreglo.palabras = new unsigned char*[capacidadInicial]; 
    arreglo.capacity = capacidadInicial; 
    arreglo.cantidad = 0; 
}

void destruirArreglo(ArregloPalabras& arreglo) {
    for (int i = 0; i < arreglo.cantidad; i++) {
        delete[] arreglo.palabras[i];
    }
    delete[] arreglo.palabras;
    arreglo.palabras = nullptr;
    arreglo.cantidad = 0;
}

void aumentarCapacidad(ArregloPalabras& arreglo) {
    int nuevaCapacidad = arreglo.capacity * 2; 
    unsigned char** nuevoArreglo = new unsigned char*[nuevaCapacidad]; 

    for (int i = 0; i < arreglo.cantidad; i++) {
        nuevoArreglo[i] = arreglo.palabras[i];
    }

    delete[] arreglo.palabras; 
    arreglo.palabras = nuevoArreglo; 
    arreglo.capacity = nuevaCapacidad; 
}

int busqueda_binaria(ArregloPalabras& arreglo, const unsigned char* palabra, int* posiciones) {
    if (arreglo.cantidad == 0) return -1;
    
    int letra = palabra[0] - 'a';
    int inicio = 0;
    int fin = arreglo.cantidad - 1; 

    if (letra >= 0 && letra < 26) {
        if (posiciones[letra] != -1) {
            inicio = posiciones[letra]; 
        } else {
            return -1;
        }
        
        for (int i = letra + 1; i < 26; i++) {
            if (posiciones[i] != -1) {
                fin = posiciones[i] - 1; 
                break;     
            }
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

void anadirPalabra(ArregloPalabras& arreglo, unsigned char* palabra, int* posiciones) {
    if (arreglo.cantidad >= arreglo.capacity) aumentarCapacidad(arreglo); 
    
    int letra = palabra[0] - 'a'; 
    int posicion = arreglo.cantidad; 

    if (letra >= 0 && letra < 26 && posiciones[letra] != -1) { 
        posicion = posiciones[letra];
    } else if (letra >= 0 && letra < 26) {
        for (int i = letra + 1; i < 26; i++) {
            if (posiciones[i] != -1) {
                posicion = posiciones[i]; 
                break;
            }
        }
    }

    while (posicion < arreglo.cantidad && compararLexicografico((char*)arreglo.palabras[posicion], (char*)palabra) < 0) {
        posicion++; 
    }

    for (int i = arreglo.cantidad; i > posicion; i--) {
        arreglo.palabras[i] = arreglo.palabras[i - 1];
    }

    arreglo.palabras[posicion] = palabra; 
    arreglo.cantidad++;

    if (letra >= 0 && letra < 26) {
        if (posiciones[letra] == -1 || posicion < posiciones[letra]) {
            posiciones[letra] = posicion;
        }
        for (int i = letra + 1; i < 26; i++) {
            if (posiciones[i] != -1) posiciones[i]++; 
        }
    }
}

void eliminarPalabra(ArregloPalabras& arreglo, const unsigned char* palabra, int* posiciones) {
    int posicion = busqueda_binaria(arreglo, palabra, posiciones);
    if (posicion == -1) return; 

    int letra = palabra[0] - 'a';

    delete[] arreglo.palabras[posicion];

    for (int i = posicion; i < arreglo.cantidad - 1; i++) {
        arreglo.palabras[i] = arreglo.palabras[i + 1];
    }
    arreglo.cantidad--;

    if (letra >= 0 && letra < 26) {
        if (posiciones[letra] == posicion) {
            if (posicion < arreglo.cantidad && arreglo.palabras[posicion][0] - 'a' == letra) {
                posiciones[letra] = posicion; 
            } else {
                posiciones[letra] = -1; 
            }
        }
        for (int i = 0; i < 26; i++) {
            if (posiciones[i] > posicion) posiciones[i]--;
        }
    }
}

void ejecutarExperimento0BusquedaSol1(ArregloPalabras& arreglo, int* posiciones, const vector<string>& palabrasD2) {
    cout << "\n Iniciando Busquedas en Arreglo Dinamico..." << endl;
    int num_busquedas = min(10000, (int)palabrasD2.size());
    int encontradas = 0;

    auto inicio = chrono::high_resolution_clock::now();
    for (int i = 0; i < num_busquedas; i++) {
        if (busqueda_binaria(arreglo, (const unsigned char*)palabrasD2[i].c_str(), posiciones) != -1) {
            encontradas++;
        }
    }
    auto fin = chrono::high_resolution_clock::now();
    double tiempo = chrono::duration<double>(fin - inicio).count();

    cout << "Busquedas exitosas: " << encontradas << " de " << num_busquedas << endl;
    cout << "Tiempo promedio de busqueda: " << scientific << (tiempo / num_busquedas) << " segundos." << endl;
}

void ejecutarExperimento0InsercionSol1(ArregloPalabras& arreglo, int* posiciones, const vector<string>& palabrasD2) {
    cout << "\n Iniciando Inserciones en Arreglo Dinamico..." << endl;
    int limiteIns = min(5000, (int)palabrasD2.size());
    
    vector<string> palabrasInsertar(palabrasD2.begin(), palabrasD2.begin() + limiteIns);
    random_device rd;
    mt19937 g(rd());
    shuffle(palabrasInsertar.begin(), palabrasInsertar.end(), g);

    auto inicio = chrono::high_resolution_clock::now();
    for (int i = 0; i < limiteIns; i++) {
        unsigned char* nueva = new unsigned char[palabrasInsertar[i].length() + 1];
        strcpy((char*)nueva, palabrasInsertar[i].c_str());
        anadirPalabra(arreglo, nueva, posiciones);
    }
    auto fin = chrono::high_resolution_clock::now();
    double tiempo = chrono::duration<double>(fin - inicio).count();

    cout << "Inserciones anadidas con exito: " << limiteIns << endl;
    cout << "Tiempo total de insercion dinamica: " << fixed << tiempo << " segundos." << endl;
}

void ejecutarExperimento0EliminacionSol1(ArregloPalabras& arreglo, int* posiciones, const vector<string>& palabrasD2) {
    cout << "\n Iniciando Eliminaciones en Arreglo Dinamico..." << endl;
    vector<string> palabrasEliminar;
    if (palabrasD2.size() > 5000) {
        palabrasEliminar.assign(palabrasD2.end() - 5000, palabrasD2.end());
    } else {
        palabrasEliminar = palabrasD2;
    }

    auto inicio = chrono::high_resolution_clock::now();
    int el = 0;
    for (const string& p : palabrasEliminar) {
        int antes = arreglo.cantidad;
        eliminarPalabra(arreglo, (const unsigned char*)p.c_str(), posiciones);
        if (arreglo.cantidad < antes) el++;
    }
    auto fin = chrono::high_resolution_clock::now();
    double tiempo = chrono::duration<double>(fin - inicio).count();

    cout << "Eliminaciones exitosas ejecutadas: " << el << endl;
    cout << "Tiempo total de eliminacion dinamica: " << fixed << tiempo << " segundos." << endl;
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Uso: ./solucion1 <Experimento_0_1_2_3> <Cantidad_n>" << endl;
        return 1;
    }

    int experimento = atoi(argv[1]);
    int cantidad = atoi(argv[2]);   
    int cantidadAInsertar = 69903;

    int posicionesIniciales[26]; 
    for(int i = 0; i < 26; i++) posicionesIniciales[i] = -1;
    
    ArregloPalabras miArreglo;
    crearArreglo(miArreglo, CAPACIDAD_INICIAL);

    vector<string> todasLasPalabrasD1;
    ifstream archivoD1("D1.txt");
    if (!archivoD1.is_open()) {
        cout << "Error: No se pudo abrir el archivo D1.txt" << endl;
        destruirArreglo(miArreglo);
        return 1;
    }
    string linea;
    while (getline(archivoD1, linea)) {
        if (!linea.empty()) todasLasPalabrasD1.push_back(linea);
    }
    archivoD1.close();

    vector<string> palabrasD2;
    ifstream archivoD2("D2.txt");
    if (!archivoD2.is_open()) {
        cout << "Error: No se pudo abrir el archivo D2.txt" << endl;
        destruirArreglo(miArreglo);
        return 1;
    }
    while (getline(archivoD2, linea)) {
        if (!linea.empty()) palabrasD2.push_back(linea);
    }
    archivoD2.close();

    if (experimento == 0) {
        cout << "--- Ejecutando codigo predeterminado (Opcion 0) - Solucion 1 ---" << endl;
        cantidadAInsertar = 69903;
    } else if (experimento == 1) {
        cantidadAInsertar = cantidad; 
    }

    if (experimento != 1) {
        for (int i = 0; i < cantidadAInsertar; i++) {
            string p = todasLasPalabrasD1[i];
            unsigned char* palabraNueva = new unsigned char[p.length() + 1];
            strcpy((char*)palabraNueva, p.c_str());
            anadirPalabra(miArreglo, palabraNueva, posicionesIniciales);
        }
    }

    if (experimento == 0) {
        ejecutarExperimento0BusquedaSol1(miArreglo, posicionesIniciales, palabrasD2);
        ejecutarExperimento0InsercionSol1(miArreglo, posicionesIniciales, palabrasD2);
        ejecutarExperimento0EliminacionSol1(miArreglo, posicionesIniciales, palabrasD2);
    }
    else if (experimento == 1) {
        cout << "Experimento 1: Creacion de la estructura (Arreglo)" << endl;
        auto inicioConst = chrono::high_resolution_clock::now();
        
        for (int i = 0; i < cantidadAInsertar; i++) {
            string p = todasLasPalabrasD1[i];
            unsigned char* palabraNueva = new unsigned char[p.length() + 1];
            strcpy((char*)palabraNueva, p.c_str());
            anadirPalabra(miArreglo, palabraNueva, posicionesIniciales);
        }
        
        auto finConst = chrono::high_resolution_clock::now();
        double tiempoTotal = chrono::duration<double>(finConst - inicioConst).count();

        size_t memoriaPalabras = 0;
        for (int i = 0; i < miArreglo.cantidad; i++) {
            memoriaPalabras += strlen((char*)miArreglo.palabras[i]) + 1;
        }
        double memoriaMB = memoriaPalabras / (1024.0 * 1024.0);

        cout << "Tiempo total de insercion: " << fixed << tiempoTotal << " segundos." << endl;
        cout << "Cantidad de palabras:      " << miArreglo.cantidad << endl;
        cout << "Espacio estimado en RAM:   " << memoriaMB << " MB." << endl;
    }
    else if (experimento == 2) {
        cout << "Experimento 2: Busqueda de palabras ya existentes" << endl;
        random_device rd;
        mt19937 gen(rd());
        shuffle(todasLasPalabrasD1.begin(), todasLasPalabrasD1.end(), gen);
        
        auto inicioBusqueda = chrono::high_resolution_clock::now();
        double promedioBusqueda = 0.0;

        int n_busquedas = min(cantidad, (int)todasLasPalabrasD1.size());
        for (int i = 0; i < n_busquedas; i++) {
            string p = todasLasPalabrasD1[i];
            auto b_ini = chrono::high_resolution_clock::now();
            busqueda_binaria(miArreglo, (const unsigned char*)p.c_str(), posicionesIniciales);
            auto b_fin = chrono::high_resolution_clock::now();
            promedioBusqueda += chrono::duration<double>(b_fin - b_ini).count();
        }
        auto finBusquedaTotal = chrono::high_resolution_clock::now();
        
        cout << "Tiempo promedio por palabra: " << scientific << (promedioBusqueda / n_busquedas) << " segundos." << endl;
        cout << "Tiempo total de busqueda:    " << chrono::duration<double>(finBusquedaTotal - inicioBusqueda).count() << " segundos." << endl;
    }
    else if (experimento == 3) {
        cout << "Experimento 3: Insercion y eliminacion intercalada desde D2" << endl;
        int inserciones = 0, eliminaciones = 0;
        int n_operaciones = min(cantidad, (int)palabrasD2.size());

        auto inicioIntercalado = chrono::high_resolution_clock::now();
        for (int i = 0; i < n_operaciones; i++) {
            if (i % 2 == 0) {   
                unsigned char* nueva = new unsigned char[palabrasD2[i].length() + 1];
                strcpy((char*)nueva, palabrasD2[i].c_str());
                anadirPalabra(miArreglo, nueva, posicionesIniciales);
                inserciones++;
            } else {
                int antes = miArreglo.cantidad;
                eliminarPalabra(miArreglo, (const unsigned char*)palabrasD2[i].c_str(), posicionesIniciales);
                if (miArreglo.cantidad < antes) eliminaciones++;
            }
        }
        auto finIntercalado = chrono::high_resolution_clock::now();
        
        cout << "Tiempo total intercalado: " << chrono::duration<double>(finIntercalado - inicioIntercalado).count() << " segundos." << endl;
        cout << "Inserciones realizadas:   " << inserciones << endl;
        cout << "Eliminaciones exitosas:   " << eliminaciones << endl;
    }

    cout << endl << "----------------- PROCESO TERMINADO -----------------" << endl;
    destruirArreglo(miArreglo);
    return 0;
}
//EJECUCION: ./solucion3 <archivo_D1> <valor_k>
//Ejemplo: ./solucion3 D1.txt 8

#include <iostream>
#include <string>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <cstring>
#include <chrono>
#include <algorithm>
#include <random>
#include "compara_ascii.cpp"

using namespace std;

const int REP = 10000; // numero de busquedas en el experimento 2


// Nodo del arbol k-ario
// Cada nodo guarda hasta k claves ordenadas y hasta k+1 punteros a hijos.

struct NodoK {
    unsigned char** claves; // arreglo de hasta k punteros a strings (claves)
    NodoK** hijos;          // arreglo de hasta k+1 punteros a nodos hijos
    int numClaves;          // cantidad de claves actualmente almacenadas
    int k;                   // capacidad maxima de claves por nodo

    NodoK(int kVal) {
        k = kVal;
        numClaves = 0;
        claves = new unsigned char*[k];
        hijos = new NodoK*[k + 1];
        for (int i = 0; i < k; i++) claves[i] = nullptr;
        for (int i = 0; i <= k; i++) hijos[i] = nullptr;
    }
};


// Funciones auxiliares

// Crea una copia dinamica de una palabra
unsigned char* copiarPalabra(const unsigned char* palabra) {
    int largo = 0;
    while (palabra[largo] != '\0') largo++;
    unsigned char* copia = new unsigned char[largo + 1];
    for (int i = 0; i <= largo; i++) copia[i] = palabra[i];
    return copia;
}

// Crea un nodo hoja con una unica clave (usado al partir nodos)
NodoK* crearNodoConClave(unsigned char* clave, int k) {
    NodoK* nodo = new NodoK(k);
    nodo->claves[0] = clave;
    nodo->numClaves = 1;
    return nodo;
}


// Busqueda en el arbol k-ario
// Devuelve true si la palabra existe.
// Complejidad: O(log_{k+1}(N) * k) para recorrer niveles y comparar dentro
// de cada nodo (busqueda interna lineal en el arreglo de k claves), donde
// N es el numero total de claves almacenadas.
bool buscar(NodoK* raiz, const unsigned char* palabra) {
    NodoK* actual = raiz;
    while (actual != nullptr) {
        int i = 0;
        // Recorre las claves del nodo de izquierda a derecha
        while (i < actual->numClaves) {
            int cmp = compararLexicografico(palabra, actual->claves[i]);
            if (cmp == 0) return true;        // encontrada
            if (cmp < 0) break;                // palabra menor: ir al hijo i
            i++;
        }
        // Descender por el hijo correspondiente
        actual = actual->hijos[i];
    }
    return false;
}

// Insercion en el arbol k-ario
//
//    arbol k-ario de busqueda sin balanceo estricto:
//  - Se busca la posicion correspondiente bajando por el arbol.
//  - Si se llega a una hoja con espacio disponible (numClaves < k),
//    se inserta ordenadamente dentro del nodo.
//  - Si la hoja esta llena, se crea un nuevo nodo hijo (hoja) que
//    contiene la nueva clave, colgando del puntero correspondiente.
//
// Esto mantiene el orden lexicografico entre hermanos y entre niveles,
// cumpliendo la propiedad de busqueda del arbol (k+1)-ario, aunque el
// arbol puede crecer de forma no perfectamente balanceada.
NodoK* insertar(NodoK* raiz, unsigned char* palabra, int k) {
    if (raiz == nullptr) {
        return crearNodoConClave(palabra, k);
    }

    NodoK* actual = raiz;
    while (true) {
        // Buscar posicion dentro del nodo actual
        int i = 0;
        bool igual = false;
        while (i < actual->numClaves) {
            int cmp = compararLexicografico(palabra, actual->claves[i]);
            if (cmp == 0) { igual = true; break; }
            if (cmp < 0) break;
            i++;
        }

        if (igual) {
            // Clave repetida: no se inserta (claves unicas)
            delete[] palabra;
            return raiz;
        }

        // Si el nodo tiene espacio libre, insertar aqui ordenadamente
        if (actual->numClaves < actual->k) {
            for (int j = actual->numClaves; j > i; j--) {
                actual->claves[j] = actual->claves[j - 1];
                actual->hijos[j + 1] = actual->hijos[j];
            }
            actual->claves[i] = palabra;
            // El nuevo hueco de hijo (i+1) hereda el subarbol que estaba en hijos[i]
            actual->hijos[i + 1] = actual->hijos[i];
            actual->numClaves++;
            return raiz;
        }

        // El nodo esta lleno: descender por el hijo i
        if (actual->hijos[i] == nullptr) {
            // No hay hijo: crear nodo hoja nuevo
            actual->hijos[i] = crearNodoConClave(palabra, k);
            return raiz;
        }
        actual = actual->hijos[i];
    }
}

// Eliminacion en el arbol k-ario
//  - Se busca el nodo que contiene la clave.
//  - Si el nodo es hoja (todos sus hijos nulos), se elimina la clave del
//    arreglo desplazando el resto.
//  - Si el nodo no es hoja, se reemplaza la clave por el sucesor
//    lexicografico (la clave mas pequena del subarbol derecho asociado,
//    es decir hijos[i+1]) y se elimina dicho sucesor recursivamente de su
//    nodo hoja, manteniendo la propiedad de orden.
// Devuelve true si se elimino una clave existente, false si la palabra
// no se encontro (las eliminaciones pueden ser no exitosas).

// Elimina y retorna la clave minima (mas a la izquierda) del subarbol,
// dejando el nodo en estado consistente.
unsigned char* extraerMinimo(NodoK* nodo) {
    NodoK* actual = nodo;
    while (actual->hijos[0] != nullptr) {
        actual = actual->hijos[0];
    }
    // 'actual' es una hoja: extraer claves[0]
    unsigned char* minimo = actual->claves[0];
    for (int j = 0; j < actual->numClaves - 1; j++) {
        actual->claves[j] = actual->claves[j + 1];
        actual->hijos[j] = actual->hijos[j + 1];
    }
    actual->hijos[actual->numClaves - 1] = actual->hijos[actual->numClaves];
    actual->claves[actual->numClaves - 1] = nullptr;
    actual->numClaves--;
    return minimo;
}

bool eliminar(NodoK* raiz, const unsigned char* palabra) {
    NodoK* actual = raiz;
    while (actual != nullptr) {
        int i = 0;
        bool igual = false;
        while (i < actual->numClaves) {
            int cmp = compararLexicografico(palabra, actual->claves[i]);
            if (cmp == 0) { igual = true; break; }
            if (cmp < 0) break;
            i++;
        }

        if (igual) {
            // Caso hoja: simplemente desplazar claves
            if (actual->hijos[i] == nullptr && actual->hijos[i + 1] == nullptr) {
                delete[] actual->claves[i];
                for (int j = i; j < actual->numClaves - 1; j++) {
                    actual->claves[j] = actual->claves[j + 1];
                }
                actual->claves[actual->numClaves - 1] = nullptr;
                actual->numClaves--;
                return true;
            }

            // Caso interno: reemplazar por el minimo del subarbol derecho (hijos[i+1])
            if (actual->hijos[i + 1] != nullptr) {
                unsigned char* sucesor = extraerMinimo(actual->hijos[i + 1]);
                delete[] actual->claves[i];
                actual->claves[i] = sucesor;
                return true;
            } else {
                // Si no hay subarbol derecho, usar el maximo del subarbol izquierdo
                NodoK* sub = actual->hijos[i];
                NodoK* padreSub = nullptr;
                while (sub->hijos[sub->numClaves] != nullptr) {
                    padreSub = sub;
                    sub = sub->hijos[sub->numClaves];
                }
                unsigned char* maximo = sub->claves[sub->numClaves - 1];
                delete[] actual->claves[i];
                actual->claves[i] = maximo;
                // remover maximo de su hoja
                sub->hijos[sub->numClaves] = sub->hijos[sub->numClaves - 1];
                sub->claves[sub->numClaves - 1] = nullptr;
                sub->numClaves--;
                return true;
            }
        }

        // No es igual: descender
        actual = actual->hijos[i];
    }
    return false; // no encontrada
}

// Calculo de memoria utilizada (aproximado)
// Recorre el arbol y suma el tamano de cada nodo (estructura + claves).
long long calcularMemoria(NodoK* raiz) {
    if (raiz == nullptr) return 0;

    long long total = sizeof(NodoK);                    // cabecera del nodo
    total += (raiz->k) * sizeof(unsigned char*);         // arreglo de claves (punteros)
    total += (raiz->k + 1) * sizeof(NodoK*);             // arreglo de hijos (punteros)

    for (int i = 0; i < raiz->numClaves; i++) {
        // largo de la clave + 1 (terminador '\0')
        int largo = 0;
        while (raiz->claves[i][largo] != '\0') largo++;
        total += (largo + 1) * sizeof(unsigned char);
    }

    for (int i = 0; i <= raiz->k; i++) {
        total += calcularMemoria(raiz->hijos[i]);
    }
    return total;
}

// MAIN: experimentos de construccion, busqueda, insercion y eliminacion
/*EJECUCION:
1.-Compilar con Makefile
2.-Ejecutar, pasandole como primer argumento el Diccionario1 (D1.txt) y como
segundo argumento el valor de k con el que quiere ejecutar (8,32,128,512)*/
int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Uso: ./solucion3 <archivo_D1> <valor_k>" << endl;
        return 1;
    }

    string archivoD1 = argv[1];
    int k = stoi(argv[2]);

    if (k <= 0) {
        cout << "Error: k debe ser un entero positivo (potencia de 2)." << endl;
        return 1;
    }

    // EXPERIMENTO 1: Construccion
    cout << "Cargando D1 y construyendo el arbol " << (k + 1) << "-ario (k=" << k << ")..." << endl;

    ifstream fileD1(archivoD1);
    if (!fileD1.is_open()) {
        cout << "Error: no se pudo abrir " << archivoD1 << endl;
        return 1;
    }

    vector<string> palabrasD1;
    string linea;
    while (getline(fileD1, linea)) {
        if (!linea.empty()) palabrasD1.push_back(linea);
    }
    fileD1.close();

    NodoK* raiz = nullptr;

    auto inicioConst = chrono::high_resolution_clock::now();
    for (const string& p : palabrasD1) {
        unsigned char* palabraNueva = copiarPalabra((const unsigned char*)p.c_str());
        raiz = insertar(raiz, palabraNueva, k);
    }
    auto finConst = chrono::high_resolution_clock::now();
    chrono::duration<double> tiempoConst = finConst - inicioConst;

    long long memoria = calcularMemoria(raiz);

    cout << "Estructura creada en: " << tiempoConst.count() << " segundos." << endl;
    cout << "Memoria total utilizada: " << memoria << " bytes ("
         << (memoria / (1024.0 * 1024.0)) << " MB)." << endl;
    cout << "Valor de k utilizado: " << k << endl;

    // EXPERIMENTO 2: Busqueda de REP claves existentes
    cout << "\nIniciando Experimento 2: Busqueda de " << REP << " claves de D1..." << endl;

    vector<string> copiaD1 = palabrasD1;
    random_device rd;
    mt19937 g(rd());
    shuffle(copiaD1.begin(), copiaD1.end(), g);

    int numBusquedas = min(REP, (int)copiaD1.size());
    int encontradas = 0;

    auto inicioBusqueda = chrono::high_resolution_clock::now();
    for (int i = 0; i < numBusquedas; i++) {
        if (buscar(raiz, (const unsigned char*)copiaD1[i].c_str())) {
            encontradas++;
        }
    }
    auto finBusqueda = chrono::high_resolution_clock::now();
    chrono::duration<double> tiempoBusqueda = finBusqueda - inicioBusqueda;
    double tiempoPromedio = tiempoBusqueda.count() / numBusquedas;

    cout << "Total busquedas: " << numBusquedas << endl;
    cout << "Palabras encontradas: " << encontradas << endl;
    cout << "Tiempo total de busqueda: " << tiempoBusqueda.count() << " segundos." << endl;
    cout << "Tiempo promedio por palabra: " << tiempoPromedio << " segundos." << endl;

    // EXPERIMENTO 3: Insercion/Eliminacion intercalada con D2 
    cout << "\nIniciando Experimento 3: Insercion/Eliminacion intercalada con D2..." << endl;

    ifstream fileD2("D2.txt");
    vector<string> palabrasD2;
    while (getline(fileD2, linea)) {
        if (!linea.empty()) palabrasD2.push_back(linea);
    }
    fileD2.close();

    int insExitosas = 0, elimExitosas = 0, elimFallidas = 0;
    double tiempoInsTotal = 0.0, tiempoElimTotal = 0.0;

    for (size_t i = 0; i < palabrasD2.size(); i++) {
        if (i % 2 == 0) {
            // Insercion
            unsigned char* palabraNueva = copiarPalabra((const unsigned char*)palabrasD2[i].c_str());
            auto t1 = chrono::high_resolution_clock::now();
            raiz = insertar(raiz, palabraNueva, k);
            auto t2 = chrono::high_resolution_clock::now();
            tiempoInsTotal += chrono::duration<double>(t2 - t1).count();
            insExitosas++;
        } else {
            // Eliminacion
            auto t1 = chrono::high_resolution_clock::now();
            bool ok = eliminar(raiz, (const unsigned char*)palabrasD2[i].c_str());
            auto t2 = chrono::high_resolution_clock::now();
            tiempoElimTotal += chrono::duration<double>(t2 - t1).count();
            if (ok) elimExitosas++;
            else elimFallidas++;
        }
    }

    cout << "Total palabras de D2 procesadas: " << palabrasD2.size() << endl;
    cout << "Inserciones realizadas: " << insExitosas << endl;
    cout << "Tiempo total de insercion: " << tiempoInsTotal << " segundos." << endl;
    cout << "Eliminaciones exitosas: " << elimExitosas << endl;
    cout << "Eliminaciones no exitosas: " << elimFallidas << endl;
    cout << "Tiempo total de eliminacion: " << tiempoElimTotal << " segundos." << endl;

    long long memoriaFinal = calcularMemoria(raiz);
    cout << "\nMemoria final utilizada: " << memoriaFinal << " bytes ("
         << (memoriaFinal / (1024.0 * 1024.0)) << " MB)." << endl;

    return 0;
}

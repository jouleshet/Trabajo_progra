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

// Elimina el \r final de una linea si existe (archivos con formato Windows CRLF)
void limpiarCR(string& s) {
    if (!s.empty() && s.back() == '\r') s.pop_back();
}

#pragma region estructuras

// Nodo del arbol k-ario:
// almacena hasta k claves ordenadas y hasta k+1 punteros a hijos.
struct NodoK {
    unsigned char** claves; // arreglo de hasta k punteros a strings (claves)
    NodoK** hijos;          // arreglo de hasta k+1 punteros a nodos hijos
    int numClaves;          // cantidad de claves actualmente almacenadas
    int k;                  // capacidad maxima de claves por nodo

    NodoK(int kVal) {
        k = kVal;
        numClaves = 0;
        claves = new unsigned char*[k];
        hijos  = new NodoK*[k + 1];
        for (int i = 0; i < k; i++)  claves[i] = nullptr;
        for (int i = 0; i <= k; i++) hijos[i]  = nullptr;
    }
};

#pragma endregion estructuras

#pragma region funciones_auxiliares

// Crea una copia dinamica de una palabra en memoria heap.
unsigned char* copiarPalabra(const unsigned char* palabra) {
    int largo = 0;
    while (palabra[largo] != '\0') largo++;
    unsigned char* copia = new unsigned char[largo + 1];
    for (int i = 0; i <= largo; i++) copia[i] = palabra[i];
    return copia;
}

// Crea un nodo hoja con una unica clave.
NodoK* crearNodoConClave(unsigned char* clave, int k) {
    NodoK* nodo = new NodoK(k);
    nodo->claves[0] = clave;
    nodo->numClaves = 1;
    return nodo;
}

#pragma endregion funciones_auxiliares

#pragma region operaciones

// BUSQUEDA iterativa en el arbol k-ario.
bool buscar(NodoK* raiz, const unsigned char* palabra) {
    NodoK* actual = raiz;
    while (actual != nullptr) {
        int i = 0;
        while (i < actual->numClaves) {
            int cmp = compararLexicografico(palabra, actual->claves[i]);
            if (cmp == 0) return true;
            if (cmp < 0)  break;
            i++;
        }
        actual = actual->hijos[i];
    }
    return false;
}

// INSERCION iterativa en el arbol k-ario.
NodoK* insertar(NodoK* raiz, unsigned char* palabra, int k) {
    if (raiz == nullptr) return crearNodoConClave(palabra, k);

    NodoK* actual = raiz;
    while (true) {
        int i = 0;
        bool igual = false;
        while (i < actual->numClaves) {
            int cmp = compararLexicografico(palabra, actual->claves[i]);
            if (cmp == 0) { igual = true; break; }
            if (cmp < 0)  break;
            i++;
        }

        if (igual) {
            delete[] palabra;
            return raiz;
        }

        if (actual->numClaves < actual->k) {
            for (int j = actual->numClaves; j > i; j--) {
                actual->claves[j]    = actual->claves[j - 1];
                actual->hijos[j + 1] = actual->hijos[j];
            }
            actual->claves[i]    = palabra;
            actual->hijos[i + 1] = actual->hijos[i];
            actual->numClaves++;
            return raiz;
        }

        if (actual->hijos[i] == nullptr) {
            actual->hijos[i] = crearNodoConClave(palabra, k);
            return raiz;
        }
        actual = actual->hijos[i];
    }
}

// Extrae y retorna la clave minima del subarbol dado (sucesor inorden).
unsigned char* extraerMinimo(NodoK* nodo) {
    NodoK* actual = nodo;
    while (actual->hijos[0] != nullptr) actual = actual->hijos[0];

    unsigned char* minimo = actual->claves[0];
    for (int j = 0; j < actual->numClaves - 1; j++) {
        actual->claves[j] = actual->claves[j + 1];
        actual->hijos[j]  = actual->hijos[j + 1];
    }
    actual->hijos[actual->numClaves - 1] = actual->hijos[actual->numClaves];
    actual->claves[actual->numClaves - 1] = nullptr;
    actual->numClaves--;
    return minimo;
}

// ELIMINACION en el arbol k-ario.
bool eliminar(NodoK* raiz, const unsigned char* palabra) {
    NodoK* actual = raiz;
    while (actual != nullptr) {
        int i = 0;
        bool igual = false;
        while (i < actual->numClaves) {
            int cmp = compararLexicografico(palabra, actual->claves[i]);
            if (cmp == 0) { igual = true; break; }
            if (cmp < 0)  break;
            i++;
        }

        if (igual) {
            if (actual->hijos[i] == nullptr && actual->hijos[i + 1] == nullptr) {
                delete[] actual->claves[i];
                for (int j = i; j < actual->numClaves - 1; j++)
                    actual->claves[j] = actual->claves[j + 1];
                actual->claves[actual->numClaves - 1] = nullptr;
                actual->numClaves--;
                return true;
            }

            if (actual->hijos[i + 1] != nullptr) {
                unsigned char* sucesor = extraerMinimo(actual->hijos[i + 1]);
                delete[] actual->claves[i];
                actual->claves[i] = sucesor;
                return true;
            }

            NodoK* sub = actual->hijos[i];
            while (sub->hijos[sub->numClaves] != nullptr)
                sub = sub->hijos[sub->numClaves];
            unsigned char* maximo = sub->claves[sub->numClaves - 1];
            delete[] actual->claves[i];
            actual->claves[i] = maximo;
            sub->hijos[sub->numClaves]      = sub->hijos[sub->numClaves - 1];
            sub->claves[sub->numClaves - 1] = nullptr;
            sub->numClaves--;
            return true;
        }

        actual = actual->hijos[i];
    }
    return false;
}

// CALCULO DE MEMORIA iterativo con pila manual.
// Se usa pila manual en vez de recursion porque con k=8 el arbol puede
// tener ~8700 niveles de profundidad, lo que desbordaria el stack del SO.
long long calcularMemoria(NodoK* raiz) {
    if (raiz == nullptr) return 0;
    long long total = 0;
    vector<NodoK*> pila;
    pila.push_back(raiz);
    while (!pila.empty()) {
        NodoK* nodo = pila.back();
        pila.pop_back();
        if (nodo == nullptr) continue;

        total += sizeof(NodoK);
        total += nodo->k * sizeof(unsigned char*);
        total += (nodo->k + 1) * sizeof(NodoK*);

        for (int i = 0; i < nodo->numClaves; i++) {
            int largo = 0;
            while (nodo->claves[i][largo] != '\0') largo++;
            total += (largo + 1) * sizeof(unsigned char);
        }

        for (int i = 0; i <= nodo->k; i++)
            if (nodo->hijos[i] != nullptr) pila.push_back(nodo->hijos[i]);
    }
    return total;
}

#pragma endregion operaciones

// ---------------------------------------------------------------------------
// MAIN
// ---------------------------------------------------------------------------
int main(int argc, char* argv[]) {
    if (argc < 4) {
        cout << "Uso: ./solucion3 <experimento> <valor_n> <valor_k>" << endl;
        cout << "  experimento 1: construccion con valor_n palabras de D1" << endl;
        cout << "  experimento 2: construccion D1 + busqueda de 10.000 claves de D2" << endl;
        cout << "  experimento 3: construccion D1 + insercion primeras 5000 de D2 + eliminacion ultimas 5000 de D2" << endl;
        return 1;
    }

    int experimento = atoi(argv[1]);
    int valor_n     = atoi(argv[2]);
    int k           = atoi(argv[3]);

    if (k <= 0) {
        cout << "Error: k debe ser un entero positivo (potencia de 2)." << endl;
        return 1;
    }

    // Cargar D1.txt limpiando \r de cada linea (formato Windows)
    ifstream fileD1("D1.txt");
    if (!fileD1.is_open()) { cout << "Error: no se pudo abrir D1.txt" << endl; return 1; }

    vector<string> palabrasD1;
    string linea;

    while (getline(fileD1, linea)) {
        limpiarCR(linea);
        if (!linea.empty()) palabrasD1.push_back(linea);
    }

    fileD1.close();

    int cantidadAInsertar = (experimento == 1) ? valor_n : (int)palabrasD1.size();

    if (cantidadAInsertar > (int)palabrasD1.size()) {
        cout << "Error: valor_n excede el total de palabras en D1 (" << palabrasD1.size() << ")." << endl;
        return 1;
    }

    // --------------- Construccion ---------------
    cout << "Construyendo arbol " << (k + 1) << "-ario (k=" << k << ") con " << cantidadAInsertar << " palabras..." << endl;

    NodoK* raiz = nullptr;
    auto inicioConst = chrono::high_resolution_clock::now();
    for (int i = 0; i < cantidadAInsertar; i++) {
        unsigned char* palabraNueva = copiarPalabra((const unsigned char*)palabrasD1[i].c_str());
        raiz = insertar(raiz, palabraNueva, k);
    }
    auto finConst = chrono::high_resolution_clock::now();
    chrono::duration<double> tiempoConst = finConst - inicioConst;

    long long memoria = calcularMemoria(raiz);

    cout << "----------------------------------------" << endl;
    cout << "Estructura creada en:    " << tiempoConst.count() << " segundos." << endl;
    cout << "Memoria total utilizada: " << memoria << " bytes ("
         << (memoria / (1024.0 * 1024.0)) << " MB)." << endl;
    cout << "Valor de k utilizado:    " << k << endl;
    cout << "----------------------------------------" << endl;

    #pragma region experimento 2
    // --------------- Experimento 2: Busqueda de 10.000 claves de D2 ---------------
    if (experimento == 2) {
        num_Busquedas = valor_n;
        cout << "\nExperimento 2: Busqueda de " << num_Busquedas;
             << " claves de D2 en estructura construida con D1..." << endl;

        ifstream fileD2("D2.txt");
        if (!fileD2.is_open()) { cout << "Error: no se pudo abrir D2.txt" << endl; return 1; }
        vector<string> palabrasD2;
        while (getline(fileD2, linea)) {
            limpiarCR(linea);
            if (!linea.empty()) palabrasD2.push_back(linea);
        }
        fileD2.close();

        random_device rd;
        mt19937 g(rd());
        shuffle(palabrasD2.begin(), palabrasD2.end(), g);

        int numBusquedas = min(num_Busquedas, (int)palabrasD2.size());
        int encontradas  = 0;

        auto inicioBusqueda = chrono::high_resolution_clock::now();
        for (int i = 0; i < numBusquedas; i++)
            if (buscar(raiz, (const unsigned char*)palabrasD2[i].c_str())) encontradas++;
        auto finBusqueda = chrono::high_resolution_clock::now();
        chrono::duration<double> tiempoBusqueda = finBusqueda - inicioBusqueda;

        cout << "Total busquedas:          " << numBusquedas << endl;
        cout << "Palabras encontradas:     " << encontradas  << endl;
        cout << "Tiempo total busqueda:    " << tiempoBusqueda.count() << " segundos." << endl;
        cout << "Tiempo promedio/palabra:  " << tiempoBusqueda.count() / numBusquedas << " segundos." << endl;
    }
    #pragma endregion experimento 2
    #pragma region experimento 3
    // --------------- Experimento 3: Insercion y Eliminacion con D2 ---------------
    if (experimento == 3) {
        cout << "\nExperimento 3: Insercion y Eliminacion con D2..." << endl;

        ifstream fileD2("D2.txt");
        if (!fileD2.is_open()) { cout << "Error: no se pudo abrir D2.txt" << endl; return 1; }
        vector<string> palabrasD2;
        while (getline(fileD2, linea)) {
            limpiarCR(linea);
            if (!linea.empty()) palabrasD2.push_back(linea);
        }
        fileD2.close();

        random_device rd;
        mt19937 g(rd());

        // Insercion: primeras 5.000 palabras de D2, desordenadas
        int limiteIns = min(5000, (int)palabrasD2.size());
        vector<string> paraInsertar(palabrasD2.begin(), palabrasD2.begin() + limiteIns);
        shuffle(paraInsertar.begin(), paraInsertar.end(), g);

        int    insExitosas    = 0;
        double tiempoInsTotal = 0.0;

        for (const string& p : paraInsertar) {
            unsigned char* palabraNueva = copiarPalabra((const unsigned char*)p.c_str());
            auto t1 = chrono::high_resolution_clock::now();
            raiz = insertar(raiz, palabraNueva, k);
            auto t2 = chrono::high_resolution_clock::now();
            tiempoInsTotal += chrono::duration<double>(t2 - t1).count();
            insExitosas++;
        }

        cout << "----------------------------------------" << endl;
        cout << "Inserciones realizadas:   " << insExitosas    << endl;
        cout << "Tiempo total insercion:   " << tiempoInsTotal << " segundos." << endl;
        cout << "----------------------------------------" << endl;

        // Eliminacion: ultimas 5.000 palabras de D2, desordenadas
        int limiteElim = min(5000, (int)palabrasD2.size());
        vector<string> paraEliminar(palabrasD2.end() - limiteElim, palabrasD2.end());
        shuffle(paraEliminar.begin(), paraEliminar.end(), g);

        int    elimExitosas    = 0, elimFallidas = 0;
        double tiempoElimTotal = 0.0;

        for (const string& p : paraEliminar) {
            auto t1 = chrono::high_resolution_clock::now();
            bool ok = eliminar(raiz, (const unsigned char*)p.c_str());
            auto t2 = chrono::high_resolution_clock::now();
            tiempoElimTotal += chrono::duration<double>(t2 - t1).count();
            if (ok) elimExitosas++;
            else    elimFallidas++;
        }

        cout << "Eliminaciones exitosas:   " << elimExitosas    << endl;
        cout << "Eliminaciones fallidas:   " << elimFallidas    << " (palabra no existia en la estructura)" << endl;
        cout << "Tiempo total eliminacion: " << tiempoElimTotal << " segundos." << endl;
        cout << "----------------------------------------" << endl;
    }
    #pragma endregion experimento 3

    return 0;
}

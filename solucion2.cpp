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

using namespace std;

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

//Funciones de Enlace
void enlaza_listas(Nodo* nodoA, Nodo* nodoB) {
    if (nodoA) nodoA->sig = nodoB;
    if (nodoB) nodoB->ant = nodoA;
}

//Funcion para evitar fugas de memoria.
void destruirGrilla(Nodo* cabezaMasAlta) {
    Nodo* nivelActual = cabezaMasAlta;

    while (nivelActual != nullptr) {
        // Guardamos el puntero al nivel de abajo antes de romper el nivel actual
        Nodo* siguienteNivelAbajo = nivelActual->inf; 
        
        Nodo* nodoHorizontal = nivelActual;
        while (nodoHorizontal != nullptr) {
            Nodo* aBorrar = nodoHorizontal;
            nodoHorizontal = nodoHorizontal->sig;
            
            // 1. Liberamos la memoria del texto dinámico uchar*
            if (aBorrar->clave != nullptr) {
                delete[] aBorrar->clave; 
            }
            // 2. Liberamos el objeto Nodo en sí
            delete aBorrar; 
        }
        
        // Saltamos de piso hacia abajo
        nivelActual = siguienteNivelAbajo; 
    }
}

// Logica de la Grilla Multinivel (Solucion 2)

// Crea los niveles superiores (L2, L3... LL) cada k nodos 
Nodo* crearNiveles(Nodo* cabezaL1, int k) {
    if (!cabezaL1) return nullptr;

    Nodo* nivelActualCabeza = cabezaL1;
    
    while (true) {
        int contadorNivel = 0;
        Nodo* temp = nivelActualCabeza;
        while(temp) { contadorNivel++; temp = temp->sig; }
        
        if (contadorNivel <= k) break; // Nivel más alto alcanzado 

        Nodo* nuevaCabezaSuperior = nullptr;
        Nodo* ultimoSuperior = nullptr;
        Nodo* actualInferior = nivelActualCabeza;
        int i = 0;

        while (actualInferior) {
            if (i % k == 0) {
                Nodo* nuevoNodoSuperior = new Nodo(actualInferior->clave);
                nuevoNodoSuperior->inf = actualInferior; 
                
                if (!nuevaCabezaSuperior) {
                    nuevaCabezaSuperior = nuevoNodoSuperior;
                } else {
                    enlaza_listas(ultimoSuperior, nuevoNodoSuperior);
                }
                ultimoSuperior = nuevoNodoSuperior;
            }
            actualInferior = actualInferior->sig;
            i++;
        }
        nivelActualCabeza = nuevaCabezaSuperior;
    }
    return nivelActualCabeza; 
}

// Algoritmo de busqueda eficiente usando los niveles 
bool buscarEnGrilla(Nodo* cabezaLL, const unsigned char* palabraBuscada) {
    Nodo* actual = cabezaLL;
    while (actual) {
        while (actual->sig && compararLexicografico(actual->sig->clave, palabraBuscada) <= 0) {
            actual = actual->sig;
        }
        if (compararLexicografico(actual->clave, palabraBuscada) == 0) return true;
        if (actual->inf) actual = actual->inf; // Baja de nivel 
        else break;
    }
    return false;
}

// Funciones de Carga Originales

Nodo* insertarOrdenado(Nodo* cabeza, const unsigned char* palabra) {
    Nodo* nuevo = new Nodo(palabra);
    if (cabeza == nullptr || compararLexicografico(palabra, cabeza->clave) < 0) {
        nuevo->sig = cabeza;
        if (cabeza != nullptr) cabeza->ant = nuevo;
        return nuevo;
    }
    Nodo* actual = cabeza;
    while (actual->sig != nullptr && compararLexicografico(palabra, actual->sig->clave) > 0) {
        actual = actual->sig;
    }
    nuevo->sig = actual->sig;
    nuevo->ant = actual;
    if (actual->sig != nullptr) (actual->sig)->ant = nuevo;
    actual->sig = nuevo;
    return cabeza;
}

Nodo* Crea_Clave(string texto) {
    ifstream archivo(texto);
    string lineas;
    Nodo* cabeza = nullptr; 
    while(getline(archivo, lineas)){
        if (lineas.empty()) continue;
        cabeza = insertarOrdenado(cabeza, (const unsigned char*)lineas.c_str());
    }
    archivo.close();
    return cabeza;
}

// Main con medición de tiempo para el informe

int main(int argc, char* argv[]) {
    if (argc < 3) {
        cout << "Uso: ./solucion2 <archivo_D1> <valor_k>" << endl;
        return 1;
    }

    string archivoD1 = argv[1];
    int k = stoi(argv[2]);

    // EXPERIMENTO 1: Construccion
    cout << "Cargando Diccionario D1 y construyendo niveles..." << endl;
    auto inicio = chrono::high_resolution_clock::now();

    Nodo* L1 = Crea_Clave(archivoD1);
    Nodo* grilla = crearNiveles(L1, k);

    auto fin = chrono::high_resolution_clock::now();
    chrono::duration<double> tiempo = fin - inicio;

    cout << "Estructura creada en: " << tiempo.count() << " segundos." << endl;
    cout << "Valor de k utilizado: " << k << endl;

    // EXPERIMENTO 2: Busqueda de 10.000 palabras
    cout << "\nIniciando Experimento 2: Busquedas..." << endl;
    
    string archivoD2 = "D2.txt"; 
    ifstream fileD2(archivoD2);
    vector<string> palabrasD2;
    string linea;

    // Cargar D2.txt en un vector
    while(getline(fileD2, linea)) {
        if(!linea.empty()) palabrasD2.push_back(linea);
    }
    fileD2.close();

    // Tomar 10.000 palabras aleatorias (mezclamos y cortamos)
    random_device rd;
    mt19937 g(rd());
    shuffle(palabrasD2.begin(), palabrasD2.end(), g);
    
    int num_busquedas = min(10000, (int)palabrasD2.size());
    int encontradas = 0;

    auto inicioBusqueda = chrono::high_resolution_clock::now();

    for (int i = 0; i < num_busquedas; i++) {
        const unsigned char* palabraTest = (const unsigned char*)palabrasD2[i].c_str();
        if (buscarEnGrilla(grilla, palabraTest)) {
            encontradas++;
        }
    }

    auto finBusqueda = chrono::high_resolution_clock::now();
    chrono::duration<double> tiempoBusqueda = finBusqueda - inicioBusqueda;
    double tiempoPromedio = tiempoBusqueda.count() / num_busquedas;

    cout << "Total busquedas: " << num_busquedas << endl;
    cout << "Palabras encontradas: " << encontradas << endl;
    cout << "Tiempo total de busqueda: " << tiempoBusqueda.count() << " segundos." << endl;
    cout << "Tiempo promedio por palabra: " << tiempoPromedio << " segundos." << endl;

    // EXPERIMENTO 3: Insercion de 5.000 claves
    cout << "\nIniciando Experimento 3: Inserciones..." << endl;
    
    // Tomar las primeras 5000 palabras de D2
    int limiteIns = min(5000, (int)palabrasD2.size());
    vector<string> palabrasInsertar(palabrasD2.begin(), palabrasD2.begin() + limiteIns);
    
    // Desordenar aleatoriamente
    shuffle(palabrasInsertar.begin(), palabrasInsertar.end(), g);

    auto inicioIns = chrono::high_resolution_clock::now();
    int insExitosas = 0;

    for(const string& p : palabrasInsertar) {
        L1 = insertarOrdenado(L1, (const unsigned char*)p.c_str());
        insExitosas++;
    }

    auto finIns = chrono::high_resolution_clock::now();
    chrono::duration<double> tiempoIns = finIns - inicioIns;

    cout << "Inserciones realizadas: " << insExitosas << endl;
    cout << "Tiempo total de insercion: " << tiempoIns.count() << " segundos." << endl;

    // EXPERIMENTO 4: Eliminacion de 5.000 claves
    cout << "\nIniciando Experimento 4: Eliminaciones..." << endl;
    
    vector<string> palabrasEliminar;
    if (palabrasD2.size() > 5000) {
        // Tomar las ultimas 5000
        palabrasEliminar.assign(palabrasD2.end() - 5000, palabrasD2.end());
    } else {
        palabrasEliminar = palabrasD2;
    }
    
    // Desordenar aleatoriamente
    shuffle(palabrasEliminar.begin(), palabrasEliminar.end(), g);

    auto inicioDel = chrono::high_resolution_clock::now();
    int elimExitosas = 0;

    for(const string& p : palabrasEliminar) {
        Nodo* actual = L1;
        while(actual) {
            // buscar el nodo exacto a eliminar en la lista base
            if (compararLexicografico(actual->clave, (const unsigned char*)p.c_str()) == 0) {
                
                // reconectar punteros para aislar el nodo
                if(actual->ant) actual->ant->sig = actual->sig;
                else L1 = actual->sig; // Si era la cabeza
                
                if(actual->sig) actual->sig->ant = actual->ant;
                
                // limpieza
                delete[] actual->clave; // liberamos el string dinámico
                delete actual;          // liberamos el objeto Nodo

                elimExitosas++;
                break; // pasamos a la siguiente palabra
            }
            actual = actual->sig;
        }
    }

    auto finDel = chrono::high_resolution_clock::now();
    chrono::duration<double> tiempoDel = finDel - inicioDel;

    cout << "Eliminaciones exitosas: " << elimExitosas << endl;
    cout << "Tiempo total de eliminacion: " << tiempoDel.count() << " segundos." << endl;


    // Limpieza de basura
    destruirGrilla(grilla);
    return 0;

}

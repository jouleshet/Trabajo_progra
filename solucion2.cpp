#if defined(_WIN32) || defined(_WIN64)
    #include <windows.h>
    #include <psapi.h> // Librería para medir ram en windows
#else
    #include <sys/resource.h> //Version de linux
#endif

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

#pragma region funciones y estructuras
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

Nodo* Crea_Clave(string texto,int limite) {
    ifstream archivo(texto);
    string lineas;
    Nodo* cabeza = nullptr; 
    int contador= 0; 
    while(getline(archivo, lineas)){
        if (lineas.empty()) continue;
        if (limite!= -1 && contador >= limite){
            break;
        }
        cabeza = insertarOrdenado(cabeza, (const unsigned char*)lineas.c_str());
        contador++;
    }
    archivo.close();
    return cabeza;
}

 // ====================================================================
// Creador de Grilla
// ====================================================================
Nodo* Creador_grilla(string archivoD1, int k, int limite, Nodo*& cabezaL1) {
    cout << "-------- Iniciando Construccion de la Grilla ---------" << endl;
    auto inicio = chrono::high_resolution_clock::now();

    // 1. Cargamos el texto y creamos la lista base L1 respetando el límite
    cabezaL1 = Crea_Clave(archivoD1, limite);
    
    // 2. Construimos las autopistas de niveles superiores
    Nodo* cabezaGrilla = crearNiveles(cabezaL1, k);

    auto fin = chrono::high_resolution_clock::now();
    chrono::duration<double> tiempo = fin - inicio;

    cout << "Estructura creada en: " << tiempo.count() << " segundos." << endl;
    cout << "Valor de k utilizado: " << k << endl;
    cout << "------------------------------------------------------" << endl;
    
    return cabezaGrilla; // Retornamos el tope de la grilla para las búsquedas
}
#pragma endregion funciones y estructuras

#pragma region experimento0
// ====================================================================
// EXPERIMENTO 1: Búsqueda masiva en la Grilla
// ====================================================================
void ejecutarExperimento0Busqueda(Nodo* grilla, vector<string> palabrasD2) {
    
    cout << "\n--- Iniciando Experimento 1: Busqueda en la Grilla ---" << endl;

    // Desordenamos aleatoriamente el vector de prueba para simular casos reales
    random_device rd;
    mt19937 g(rd());
    shuffle(palabrasD2.begin(), palabrasD2.end(), g);
    
    int num_busquedas = min(10000, (int)palabrasD2.size());
    int encontradas = 0;

    auto inicio = chrono::high_resolution_clock::now();
    
    for (int i = 0; i < num_busquedas; i++) {
        const unsigned char* palabraTest = (const unsigned char*)palabrasD2[i].c_str();
        if (buscarEnGrilla(grilla, palabraTest)) {
            encontradas++;
        }
    }

    auto fin = chrono::high_resolution_clock::now();
    double tiempo = chrono::duration<double>(fin - inicio).count();

    cout << "------------------------------------------------------" << endl;
    cout << "Total de busquedas: " << num_busquedas << endl;
    cout << "Palabras encontradas: " << encontradas << endl;
    cout << "Tiempo promedio por palabra: " << scientific << (tiempo / num_busquedas) << " segundos." << endl;
    cout << "O en formato decimal: " << fixed << ((tiempo / num_busquedas) * 1e6) << " microsegundos." << endl;
    cout << "------------------------------------------------------" << endl;
}

// ====================================================================
// EXPERIMENTO 2: Inserciones dinámicas en caliente
// ====================================================================
void ejecutarExperimento0Insercion(Nodo*& cabezaL1, vector<string> palabrasD2) {
    cout << "\n--- Iniciando Experimento 2: Inserciones dinamicas ---" << endl;
    
    int limiteIns = min(5000, (int)palabrasD2.size());
    vector<string> palabrasInsertar(palabrasD2.begin(), palabrasD2.begin() + limiteIns);
    random_device rd;
    mt19937 g(rd());
    shuffle(palabrasInsertar.begin(), palabrasInsertar.end(), g);

    auto inicio = chrono::high_resolution_clock::now();
    int insExitosas = 0;

    for(const string& p : palabrasInsertar) {
        cabezaL1 = insertarOrdenado(cabezaL1, (const unsigned char*)p.c_str());
        insExitosas++;
    }

    auto fin = chrono::high_resolution_clock::now();
    double tiempo = chrono::duration<double>(fin - inicio).count();

    cout << "Inserciones realizadas: " << insExitosas << endl;
    cout << "Tiempo total de insercion: " << tiempo << " segundos." << endl;
}

// ====================================================================
// EXPERIMENTO 3: Eliminaciones dinámicas con propagación vertical
// ====================================================================
void ejecutarExperimento0Eliminacion(Nodo*& cabezaL1, vector<string> palabrasD2) {
    cout << "\n--- Iniciando Experimento 3: Eliminaciones dinamicas ---" << endl;
    
    vector<string> palabrasEliminar;
    if (palabrasD2.size() > 5000) {
        palabrasEliminar.assign(palabrasD2.end() - 5000, palabrasD2.end());
    } else {
        palabrasEliminar = palabrasD2;
    }
    
    random_device rd;
    mt19937 g(rd());
    shuffle(palabrasEliminar.begin(), palabrasEliminar.end(), g);

    auto inicio = chrono::high_resolution_clock::now();
    int elimExitosas = 0;

    for(const string& p : palabrasEliminar) {
        Nodo* actual = cabezaL1;
        while(actual) {
            if (compararLexicografico(actual->clave, (const unsigned char*)p.c_str()) == 0) {
                Nodo* subeNivel = actual;
                Nodo* reemplazo = actual->sig; 

                if (actual->ant == nullptr && actual->sig == nullptr) {
                    reemplazo = nullptr;
                }

                if(actual->ant) actual->ant->sig = actual->sig;
                else cabezaL1 = actual->sig; 
                
                if(actual->sig) actual->sig->ant = actual->ant;

                delete[] actual->clave; 
                delete actual; 

                elimExitosas++;
                break; 
            }
            actual = actual->sig;
        }
    }

    auto fin = chrono::high_resolution_clock::now();
    double tiempo = chrono::duration<double>(fin - inicio).count();

    cout << "Eliminaciones exitosas: " << elimExitosas << endl;
    cout << "Tiempo total de eliminacion: " << tiempo << " segundos." << endl;
}

#pragma endregion experimento0
#pragma region experimentos

//Funcion para medir memoria usada
long getMemoryUsageKB() {
#if defined(_WIN32) || defined(_WIN64)
    // Código específico para medir la RAM física consumida en Windows
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize / 1024; // Convertimos de Bytes a Kilobytes
    }
    return 0;
#else
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss; 
#endif
}

void EjecutarExperimento1(string archivoD1,int valor_n, int valor_k,Nodo*& cabezaL1, Nodo*& cabezaGrilla) {
    cout << "\n--- Iniciando Experimento 1: Creacion de la Grilla ---" << endl;
    auto inicio = chrono::high_resolution_clock::now();
    long memoria_inicio=getMemoryUsageKB();

    cabezaL1 = Crea_Clave(archivoD1, valor_n);
    cabezaGrilla = crearNiveles(cabezaL1, valor_k);

    long memoria_final=getMemoryUsageKB();

    long valormemoria=memoria_final-memoria_inicio;
    auto fin = chrono::high_resolution_clock::now();
    chrono::duration<double> tiempo = fin - inicio;

    cout << "Estructura creada en: " << tiempo.count() << " segundos." << endl;
    cout << "Valor de k utilizado: " << valor_k << endl;
    cout << "Mmeoria Ram usada:"<< valormemoria << "KB"<< endl;
    cout << "------------------------------------------------------" << endl;
}

void EjecutarExperimento2(Nodo* grilla, string archivoD1, int valor_n, int rep) {

    cout << "\n--- Iniciando Experimento 2: Busqueda de Claves Existentes ---" << endl;

    ifstream fileD1(archivoD1);
    vector<string> todasLasPalabras;
    string linea;
    int contador = 0;

    if (!fileD1.is_open()) {
        cout << "Error: No se pudo leer " << archivoD1 << " para extraer claves de prueba." << endl;
        return;
    }

    while (getline(fileD1, linea)) {
        if (linea.empty()) continue;
        if (contador >= valor_n) break;
        todasLasPalabras.push_back(linea);
        contador++;
    }
    fileD1.close();

    if (todasLasPalabras.empty()) {
        cout << "Error: No hay palabras cargadas para buscar." << endl;
        return;
    }

    int num_busquedas = min(rep, (int)todasLasPalabras.size());

    random_device rd;
    mt19937 g(rd());
    shuffle(todasLasPalabras.begin(), todasLasPalabras.end(), g);

    vector<string> clavesA_Buscar(todasLasPalabras.begin(), todasLasPalabras.begin() + num_busquedas);

    int encontradas = 0;

    auto inicio = chrono::high_resolution_clock::now();

    for (int i = 0; i < num_busquedas; i++) {
        const unsigned char* palabraTest = (const unsigned char*)clavesA_Buscar[i].c_str();
        if (buscarEnGrilla(grilla, palabraTest)) {
            encontradas++;
        }
    }

    auto fin = chrono::high_resolution_clock::now();
    chrono::duration<double> tiempoTotal = fin - inicio;

    double tiempoPromedio = tiempoTotal.count() / num_busquedas;

    cout << "Claves programadas (REP):      " << rep << endl;
    cout << "Claves buscadas con exito:     " << num_busquedas << endl;
    cout << "Claves encontradas en grilla:  " << encontradas << " (Debe ser igual a las buscadas)" << endl;
    cout << "Tiempo total de CPU de busqueda: " << tiempoTotal.count() << " segundos." << endl;
    cout << "Tiempo promedio por palabra:   " << scientific << tiempoPromedio << " segundos." << endl;
    cout << "------------------------------------------------------" << endl;

}

void EjecutarExperimento3(Nodo*& cabezaL1, string archivoD2) {
    cout << "\n--- Iniciando Experimento 3: Operaciones Intercaladas (D2) ---" << endl;

    ifstream fileD2(archivoD2);
    string linea;
    
    if (!fileD2.is_open()) {
        cout << "Error: No se pudo abrir el archivo " << archivoD2 << " para las operaciones intercaladas." << endl;
        return;
    }

    int total_lineas = 0;
    int insExitosas = 0;
    int elimExitosas = 0;

    auto inicio = chrono::high_resolution_clock::now();

    while (getline(fileD2, linea)) {
        if (linea.empty()) continue;
        total_lineas++;

        const unsigned char* palabraD2 = (const unsigned char*)linea.c_str();

        if (total_lineas % 2 != 0) {
            cabezaL1 = insertarOrdenado(cabezaL1, palabraD2);
            insExitosas++;
        } 
        else {
            Nodo* actual = cabezaL1;
            while (actual) {
                if (compararLexicografico(actual->clave, palabraD2) == 0) {
                    if (actual->ant) {
                        actual->ant->sig = actual->sig;
                    } else {
                        cabezaL1 = actual->sig; 
                    }
                    
                    if (actual->sig) {
                        actual->sig->ant = actual->ant;
                    }
                    
                    delete[] actual->clave;
                    delete actual;
                    
                    elimExitosas++;
                    break; 
                }
                actual = actual->sig;
            }
        }
    }
    fileD2.close();

    auto fin = chrono::high_resolution_clock::now();
    chrono::duration<double> tiempoTotal = fin - inicio;

    cout << "Total de lineas procesadas desde D2: " << total_lineas << endl;
    cout << "Operaciones de Insercion llamadas:   " << insExitosas << endl;
    cout << "Operaciones de Eliminacion exitosas: " << elimExitosas << " (De un total de " << (total_lineas / 2) << ")" << endl;
    cout << "Tiempo total de ejecucion (Exp 3):   " << tiempoTotal.count() << " segundos." << endl;
    cout << "------------------------------------------------------" << endl;
}
#pragma endregion experimentos





// Main con medición de tiempo para el informe
    /*EJECUCION:
    a.-Compilar con Makefile
    b.-Ejecutar, pasandole como primer argumento el diccionario D1.txt, el segundo
    el limite de palabras que buscas insertar, como tercer argumento el valor de k
    con el que quiere ejecutar (8,32,128,512), y finalmente el experimento
    que quieres hacer los cuales son:
    1.-Busqueda Masiva de palabras aleatorias en la grilla
    2.-Insercion de palabras aleatorias a la grilla
    3.-Eliminacion aleatoria de palabras en la grilla*/

int main(int argc, char* argv[]) {
    if (argc < 5) {
        cout << "Error: Faltan argumentos de ejecucion." << endl;
        cout << "Uso: ./solucion2 <archivo_D1> <limite_n> <valor_k> <experimento_1_2_3>" << endl;
        return 1;
    }

    cout << endl << "----------------- INICIANDO PROGRAMA -----------------" << endl;

    string archivoD1 = argv[1];
    int n = stoi(argv[2]);
    int k = stoi(argv[3]);
    int eleccion = stoi(argv[4]);
    string archivoD2 = "D2.txt";

    ifstream fileD2(archivoD2);
    vector<string> palabrasD2;
    string linea;

    if (!fileD2.is_open()) {
        cout << "Error: No se pudo abrir el archivo complementario " << archivoD2 << endl;
        return 1;
    }

    while(getline(fileD2, linea)) {
        if(!linea.empty()) palabrasD2.push_back(linea);
    }
    fileD2.close();
    
    Nodo* L1 = nullptr;    
    Nodo* grilla = nullptr;
    
    

    cout << endl;

    if (eleccion == 0){
        grilla = Creador_grilla(archivoD1, k, 69903, L1);
        ejecutarExperimento0Busqueda(grilla, palabrasD2);
        ejecutarExperimento0Insercion(L1, palabrasD2);
        ejecutarExperimento0Eliminacion(L1, palabrasD2);
    }
    else if (eleccion == 1){
        EjecutarExperimento1(archivoD1,n,k,L1,grilla);
    }

    else if (eleccion == 2){
        cout << "Ingrese REP que desea usar:";
        int rep;
        cin >> rep;
        grilla = Creador_grilla(archivoD1, k, n, L1);
        EjecutarExperimento2(grilla, archivoD1, n, rep);
    }
    else if (eleccion == 3){
        grilla = Creador_grilla(archivoD1, k, n, L1);
        EjecutarExperimento3(L1, archivoD2);
    }
    else {
        cout << "Error: Opcion de experimento invalida (Debe ser 0, 1, 2 o 3)." << endl;
    }

    cout << endl << "----------------- PROGRAMA FINALIZADO -----------------" << endl<< endl;
    // Limpieza de basura
    destruirGrilla(grilla);
    return 0;
}
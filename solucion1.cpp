
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
#pragma region funciones 
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

#pragma endregion funciones

int main(int argc, char* argv[]) {
    // 1. Validar si el usuario paso el argumento por consola
    if (argc < 3) {
        cout << "Uso: ./solucion1 <Experimento> <Cantidad>" << endl;
        return 1;
    }

    int experimento = atoi(argv[1]);
    int cantidad = atoi(argv[2]);   
    int cantidadAInsertar = 69903;
    
    cout << endl;
    // region del codigo para la creacion de estructuras, la parte final no se ejecuta si experimento es 1, ya que utiliza el codigo de experimento 1
    #pragma region creacion de la estructura
    // 2. Convertir el argumento de texto a un numero entero
    if (experimento == 1) {
        cantidadAInsertar = cantidad; // valor definido por el usuario );
    }

    if (cantidadAInsertar <= 0) {
        cout << "Error: La cantidad de palabras debe ser un numero entero mayor que 0." << endl;
        return 1;
    }

    // Inicializar el índice de letras en -1
    for (int i = 0; i < 26; i++) indice[i] = -1;
    
    // Crear la estructura de palabras
    ArregloPalabras miArreglo;
    crearArreglo(miArreglo, CAPACIDAD_INICIAL);

    // 3. Leer todas las palabras de D1.txt
    vector<string> todasLasPalabrasD1;
    ifstream archivoD1("D1.txt");
    
    if (!archivoD1.is_open()) {
        cout << "Error: No se pudo abrir el archivo D1.txt" << endl;
        return 1;
    }

    char temp[100];
    while (archivoD1.getline(temp, 100)) {
        if (strlen(temp) > 0) {
            todasLasPalabrasD1.push_back(temp);
        }
    }
    archivoD1.close();

    // Validar que el archivo tenga suficientes palabras
    if ((int)todasLasPalabrasD1.size() < cantidadAInsertar) {
        cout << "Error: D1.txt solo tiene " << todasLasPalabrasD1.size() 
             << " palabras, pero pediste " << cantidadAInsertar << "." << endl;
        destruirArreglo(miArreglo);
        return 1;
    }

    if (experimento != 1) {
        for (int i = 0; i < cantidadAInsertar; i++) {
            string p = todasLasPalabrasD1[i];
            unsigned char* palabraNueva = new unsigned char[p.length() + 1];
            strcpy((char*)palabraNueva, p.c_str());
        
            anadirPalabra(miArreglo, palabraNueva, indice);
        }
    }
    cout << "Insertando " << cantidadAInsertar << " palabras de D1.txt..." << endl << endl ;
    

    #pragma endregion creacion de la estructura 
    
    // Si experimento=1, mide el tiempo de insercion, sino, no lo mide.
    if (experimento == 1) {

        auto inicioConst = chrono::high_resolution_clock::now();
        
        for (int i = 0; i < cantidadAInsertar; i++) {

            string p = todasLasPalabrasD1[i];
            unsigned char* palabraNueva = new unsigned char[p.length() + 1];
            strcpy((char*)palabraNueva, p.c_str());
            
            anadirPalabra(miArreglo, palabraNueva, indice);
        }
        
        auto finConst = chrono::high_resolution_clock::now();

        // 6. Mostrar resultados
        double tiempoTotal = chrono::duration<double>(finConst - inicioConst).count();
        cout << "----------------------------------------- " << endl;
        cout << "Tiempo total de ordenamiento: " << scientific << tiempoTotal << " segundos." << endl;
        cout << "O en formato decimal: " << fixed << tiempoTotal << " segundos." << endl;
        cout << "----------------------------------------- " << endl;
    }

    
    if (experimento == 2) {
        
        cout << "Experimento 2: Busqueda de palabras ya existentes" << endl;
        cout << "Se buscaran " << cantidad << " palabras ya existentes en d1" << endl << endl;
        
        if ((int)todasLasPalabrasD1.size() < cantidad) {
        cout << "Error: D1.txt solo tiene " << todasLasPalabrasD1.size() 
             << " palabras, pero pediste " << cantidad << "." << endl;
        destruirArreglo(miArreglo);
        return 1;
        }

        //crear semilla random
        random_device rd;
        mt19937 gen(rd());

        //aleatorizar d1 para que busque palabras al azar
        shuffle(todasLasPalabrasD1.begin(),todasLasPalabrasD1.end(),gen);
        
        auto inicioBusqueda = chrono::high_resolution_clock::now();
        double promedioBusqueda = 0.0;

        for (int i = 0; i < cantidad; i++) {
            
            // parte del for para las palabrrars
            string p = todasLasPalabrasD1[i];
            unsigned char* palabraBuscar = new unsigned char[p.length() + 1];
            strcpy((char*)palabraBuscar, p.c_str());

            //parte del for SOLO de la busqueda, el tiempo se tiene que medir aca
            auto InicioBusquedaActual = chrono::high_resolution_clock::now();

            int resultado = busqueda_binaria(miArreglo, palabraBuscar, indice);
            if (resultado == -1) {
                cout << "Error: No se encontró la palabra '" << p << "' que debería existir." << endl;
            }   

            auto finBusquedaActual = chrono::high_resolution_clock::now();
            double tiempoBusqueda = chrono::duration<double>(finBusquedaActual - InicioBusquedaActual).count();
            
            promedioBusqueda += tiempoBusqueda;

            delete[] palabraBuscar;     
         }
        auto finBusquedaTotal = chrono::high_resolution_clock::now();
        promedioBusqueda /= cantidad;
        
        double tiempoTotal = chrono::duration<double>(finBusquedaTotal - inicioBusqueda).count();
        double promedioMicrosegundos = promedioBusqueda * 1e6;


        cout << "--------------Tiempos--------------------------- " << endl  << endl;
        cout << "Tiempo promedio de busqueda: " << scientific << promedioBusqueda << " segundos." << endl;
        cout << "O en formato decimal: " << fixed << promedioMicrosegundos << " microsegundos." << endl;
        cout << "----------------------------------------- " << endl;
        cout << "Tiempo total de busqueda: " << scientific << tiempoTotal << " segundos." << endl;
        cout << "O en formato decimal: " << fixed << tiempoTotal << " segundos." << endl;
    }



    // cout para que quede mas ordenada la consola
    cout << endl << endl << endl;
    // Liberar memoria
    destruirArreglo(miArreglo);
    return 0;
}
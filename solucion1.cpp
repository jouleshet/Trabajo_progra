/*QUE HACER
   CREAR ESTRUCTURA QUE GUARDA PALABRAS QUE FUNCIONE SIMILAR A UN ARREGLO , EL CUAL TIENE QUE ESTAR SIEMPRE ORDENADO
   EL ARREGLO TIENE CAPACIDAD MAXIMA, SI SE LLENA CREAR OTRO, QUE TENGA MAYOR CAPACIDAD, PERO QUE EL AUMENTO SEA
   DECENTE, OSEA, NO AUMENTARLO EN 1 PQ SINO CADA PALABRA HARIA OTRO MAS esto es el OVERHEAD

   usar un INDICE que guarde las posiciones de las letras, asi al buscar un elemento solo uso el valor del indice de la 1ra letra y la 
*/

#include <string>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <cstring>
#include <chrono> // <-- Librería para medir tiempos de ejecución

using namespace std;

int indice[26]; // indice para cada letra del alfabeto, asumiendo que solo se usan letras minusculas
const int CAPACIDAD_INICIAL = 100; // capacidad inicial del arreglo

// Función externa asumida en el diff para comparar strings
int compararLexicografico(const char* str1, const char* str2) {
    return strcmp(str1, str2);
}

struct ArregloPalabras { 
      unsigned char** palabras; // arreglo de punteros a palabras
      int capacidad; // capacidad maxima del arreglo
      int cantidad; // numero elementos en el arreglo
};

void crearArreglo(ArregloPalabras& arreglo, int capacidadInicial) {
   arreglo.palabras = new unsigned char*[capacidadInicial]; // reservar memoria para el arreglo de punteros
   arreglo.capacidad = capacidadInicial; // establecer la capacidad inicial
   arreglo.cantidad = 0; // iniciar la cantidad en 0
}

void destruirArreglo(ArregloPalabras& arreglo) {
    // esta funcion borra TODAS las palabras y luego el arreglo
    for (int i = 0; i < arreglo.cantidad; i++) {
        delete[] arreglo.palabras[i];
    }
    delete[] arreglo.palabras;
}

void aumentarCapacidad(ArregloPalabras& arreglo) {
   int nuevaCapacidad = arreglo.capacidad * 2; // la nueva capacidad es el doble de la actual [overhead]
   unsigned char** nuevoArreglo = new unsigned char*[nuevaCapacidad]; 

   // copiar las palabras del arreglo antiguo al nuevo
   for (int i = 0; i < arreglo.cantidad; i++) {
      nuevoArreglo[i] = arreglo.palabras[i];
   }

   delete[] arreglo.palabras; // liberar la memoria del arreglo antiguo
   arreglo.palabras = nuevoArreglo; // apuntar al nuevo arreglo
   arreglo.capacidad = nuevaCapacidad; // actualizar la capacidad
}

int busqueda_binaria(ArregloPalabras& arreglo, unsigned char* palabra, int* indice) {
   // busqueda binaria para buscar palabra en el arreglo
   // si el indice de la 1ra letra existe inicia ahi
   int letra = palabra[0] - 'a';
   int inicio = 0;
   int fin = arreglo.cantidad - 1; 

   if (indice[letra] != -1) {
      inicio = indice[palabra[0] - 'a']; 
   }
   
   // revisa el indice desde letra hasta el final, la 1ra q existe la toma como fin, sino fin sigue siendo el final
   for (int i = letra + 1; i < 26; i++) {
         if (indice[i] != -1) {
            fin = indice[i] - 1; 
            break;     
         }
   }
      
   while (inicio <= fin) {
      int medio = inicio + (fin - inicio) / 2;
      int comparacion = compararLexicografico((char*)arreglo.palabras[medio], (char*)palabra);

      if (comparacion == 0) {
         return medio;
      } else if (comparacion < 0) {
         inicio = medio + 1;
      } else {
         fin = medio - 1;
      }
   }
   return -1; // Elemento no encontrado
}

void anadirPalabra(ArregloPalabras& arreglo, unsigned char* palabra, int* indice) {
   if (arreglo.cantidad >= arreglo.capacidad) {
      aumentarCapacidad(arreglo); // si el arreglo esta lleno, aumentar su capacidad
   }
   int posicion = arreglo.cantidad; 
   int letra = palabra[0] - 'a'; // obtener la primera letra de la palabra y convertirla a un indice (0-25)

   if (indice[letra] != -1) { 
      posicion = indice[letra];
   } else {
      for (int i = letra + 1; i < 26; i++) {
         if (indice[i] != -1) {
            posicion = indice[i]; 
            break;
         }
      }
   }

   while (posicion < arreglo.cantidad &&
         strcmp((char*)arreglo.palabras[posicion], (char*)palabra) < 0) {
         posicion++; 
   }

   // Desplazamiento manual necesario para insertar manteniendo el orden (corregido de tu diff)
   for (int i = arreglo.cantidad; i > posicion; i--) {
      arreglo.palabras[i] = arreglo.palabras[i - 1];
   }

   arreglo.palabras[posicion] = palabra; // Insertar la palabra en posición
   arreglo.cantidad++;

   if (indice[letra] == -1 || posicion < indice[letra]) {
      indice[letra] = posicion; 
   }
   
   for (int i = letra + 1; i < 26; i++) {
      if (indice[i] != -1) {
         indice[i]++; 
      }
   }   
}

void eliminarPalabra(ArregloPalabras& arreglo, unsigned char* palabra, int* indice) {
   int posicion = busqueda_binaria(arreglo, palabra, indice);
   if (posicion == -1) {
      return; // Palabra no encontrada
   }

   int letra = palabra[0] - 'a';
   if (indice[letra] == posicion) {
      // Si era la única palabra con esa letra, o la primera, se debe manejar su consistencia
      if (posicion + 1 < arreglo.cantidad && arreglo.palabras[posicion + 1][0] - 'a' == letra) {
          indice[letra] = posicion; // Sigue habiendo palabras con esta letra
      } else {
          indice[letra] = -1; // No quedan palabras con esta letra
      }
   }

   // Liberar la memoria de la palabra eliminada
   delete[] arreglo.palabras[posicion];

   // Mover las palabras siguientes una posición hacia atrás
   for (int i = posicion; i < arreglo.cantidad - 1; i++) {
      arreglo.palabras[i] = arreglo.palabras[i + 1];
   }
   arreglo.cantidad--;

   // Restar 1 a los índices de las letras que siguen, si existen
   for (int i = letra + 1; i < 26; i++) {
      if (indice[i] != -1) {
         indice[i]--;
      }
   }
}

void cargarDiccionario(const string& nombreArchivo, ArregloPalabras& arreglo, int* indice) {
   ifstream archivo(nombreArchivo);

   if (!archivo.is_open()) {
      cout << "Error al abrir el archivo: " << nombreArchivo << endl;
      return;
   }

   // --- INICIO DE LA MEDICIÓN DE TIEMPO DE CONSTRUCCIÓN ---
   auto inicio = chrono::high_resolution_clock::now();

   char temp[100]; 
   while (archivo.getline(temp, 100)) {
        if (strlen(temp) == 0) continue; // Evitar líneas vacías

        unsigned char* palabra = new unsigned char[strlen(temp) + 1];
        strcpy((char*)palabra, temp);

        anadirPalabra(arreglo, palabra, indice);
   }

   archivo.close();

   // --- FIN DE LA MEDICIÓN DE TIEMPO DE CONSTRUCCIÓN ---
   auto fin = chrono::high_resolution_clock::now();
   
   // Duración en microsegundos (puedes usar milisegundos si el archivo es gigante cambiando a chrono::milliseconds)
   auto duracion = chrono::duration_cast<chrono::microseconds>(fin - inicio).count();

   cout << "Tiempo total de construccion: " << duracion << " microsegundos." << endl;
}

int main() {
   // Inicializar el índice
   for (int i = 0; i < 26; i++) {
      indice[i] = -1;
   }

   ArregloPalabras miArreglo;
   crearArreglo(miArreglo, CAPACIDAD_INICIAL);

   // Ejecución del experimento 1 (Construcción desde cero con D1.txt)
   cargarDiccionario("D1.txt", miArreglo, indice);

   // Limpieza antes de salir
   destruirArreglo(miArreglo);

   return 0;
}

/*QUE HACER
   CREAR ESTRUCTURA QUE GUARDA PALABRAS QUE FUNCIONE SIMILAR A UN ARREGLO , EL CUAL TIENE QUE ESTAR SIEMPRE ORDENADO
   EL ARREGLO TIENE CAPACIDAD MAXIMA, SI SE LLENA CREAR OTRO, QUE TENGA MAYOR CAPACIDAD, PERO QUE EL AUMENTO SEA
   DECENTE, OSEA, NO AUMENTARLO EN 1 PQ SINO CADA PALABRA HARIA OTRO MAS esto es el OVERHEAD

   usar un INDICE que guarde las posiciones de las letras, asi al buscar un elemento solo uso el valor del indice de la 1ra letra y la 
   letra siguiente, esto es la BUSQUEDA BINARIA ACOTADA

*/
#include <string>
#include <iostream>
using namespace std;
#include <cstdlib>
#include <vector>
#include <fstream>
#include <cstring>
#include <compara_ascii.h>


int indice[26]; // indice para cada letra del alfabeto, asumiendo que solo se letras minusculas
const int CAPACIDAD_INICIAL = 100; // capacidad inicial del arreglo

struct ArregloPalabras{ 
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
//esta funcion borra TODAS las palabras y luego el arreglo
    for (int i = 0; i < arreglo.cantidad; i++) {
        delete[] arreglo.palabras[i];
    }

    delete[] arreglo.palabras;
}

void aumentarCapacidad(ArregloPalabras& arreglo) {
   int nuevaCapacidad = arreglo.capacidad * 2; // la nueva capacidad esel doble de la actual, quiza cambiar por algo mas chico
   unsigned char** nuevoArreglo = new unsigned char*[nuevaCapacidad]; 

   // copiar las palabras del arreglo antiguo al nuevo
   for (int i = 0; i < arreglo.cantidad; i++) {
      nuevoArreglo[i] = arreglo.palabras[i];
   }

   delete[] arreglo.palabras; // liberar la memoria del arreglo antiguo
   arreglo.palabras = nuevoArreglo; // apuntar al nuevo arreglo
   arreglo.capacidad = nuevaCapacidad; // actualizar la capacidad
}

int busqueda_binaria( ArregloPalabras& arreglo, unsigned char* palabra, int* indice) {
   // busqueda binaria para buscar palabra en el arreglo
   //si el indice de la 1ra letra existe inicia ahi
   int letra=palabra[0]-'a';
   int inicio=0;
   int fin=arreglo.cantidad-1; 

   if (indice[letra] != -1) {
      inicio = indice[palabra[0] - 'a']; 
   }
   
   //revisa el indice desde letra hasta el final, la 1ra q existe la toma como fin, sino fin sigue siendo el final
   for (int i=letra+1;i<26;i++){
         if (indice[i] != -1) {
            fin = indice[i]-1; 
            break;     
         }
      }
      
   while (inicio <= fin){
      int medio = inicio + (fin - inicio) / 2;
      //strcmp compara la palabra de medio con la palabra buscada y devuelve un numero,
      //0 si son iguales, un neg si es menor, y un pos si es mayor.
      int comparacion= compararLexicografico((char*)arreglo.palabras[medio], (char*)palabra);
      
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

void anadirPalabra( ArregloPalabras& arreglo, unsigned char* palabra, int* indice){
   /*
   esta funcion recibe el ARREGLO de palabras, una palabra nueva, y el indice con las posiciones de las letras
   tiene que insetrar la palabra de manera que se mantenga el orden, y actualizar el indice si es necesario.
   Si supera el maximo del ARREGLO, llamar a funcion para crear la copia mas grande (aun no existe).

   */
   if (arreglo.cantidad >= arreglo.capacidad) {
      aumentarCapacidad(arreglo); // si el arreglo esta lleno, aumentar su capacidad
   }
   int posicion = arreglo.cantidad; 
   // variable de la pos donde se insertara la palabra, se incia en le final de la lista
   int letra=palabra[0]-'a'; // obtener la primera letra de la palabra y convertirla a un indice (0-25)

   if (indice[letra] != -1) { // si el indice tiene una posicion para la letra, posicion toma ese valor
      posicion = indice[letra];
   } else {
      // revisa los indices siguientes, el primero que exista, coloca la palabra ahi
      for (int i=letra+1;i<26;i++){
         if (indice[i] != -1) {
            posicion = indice[i]; 
            break;
         }
      }
   }

   while(posicion < arreglo.cantidad &&
         strcmp((char*)arreglo.palabras[posicion], (char*)palabra) < 0){
         posicion++; //While para  buscar el lugar para poner la palabra
         }

   arreglo.palabras[posicion] = palabra; // Insertar la palabra en posición
   arreglo.cantidad++;

   // tomar 1ra letra de la palabra y actualizar el indice, en int pq es ascii

   if (indice[letra] == -1 || posicion < indice[letra]) {
      indice[letra] = posicion; // Actualizar el índice para la letra
   }
   // sumarle 1 a los indices de las letras que siguen, si existen
   for (int i = letra + 1; i < 26; i++) {
      if (indice[i] != -1) {
         indice[i]++; // 
      }
   }   
}

void eliminarPalabra( ArregloPalabras& arreglo, unsigned char* palabra, int* indice) {
   int posicion = busqueda_binaria(arreglo, palabra, indice);
   if (posicion == -1) {
      return; // Palabra no encontrada
   }

   // Actualizar el índice para la letra correspondiente
   int letra = palabra[0] - 'a';
   if (indice[letra] == posicion) {
      indice[letra] = -1; // Marcar como no asignado
   }

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

   //da mensaje de error si el archivo no se pudo abrir
   if (!archivo.is_open()) {
      cout << "Error al abrir el archivo: " << nombreArchivo << endl;
      return;
     }
   char temp[100]; // temp para guardar la palabra, max 100 caracteres
       while(archivo.getline(temp, 100)){

        // reservar memoria para la palabra (temp+1 por el caracter null)
        unsigned char* palabra =
            new unsigned char[strlen(temp) + 1];

        // copiar palabra
        strcpy((char*)palabra, temp);

        // meter la palabra al vector
        anadirPalabra(
            arreglo,
            palabra,
            indice
        );
    }

    archivo.close();

}
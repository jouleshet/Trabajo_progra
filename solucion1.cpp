/*QUE HACER
   CREAR ESTRUCTURA QUE GUARDA PALABRAS EN UN ARREGLO O VECTOR, EL CUAL TIENE QUE ESTAR SIEMPRE ORDENADO
   EL VECTOR TIENE CAPACIDAD MAXIMA, SI SE LLENA CREAR OTRO, QUE TENGA MAYOR CAPACIDAD, PERO QUE EL AUMENTO SEA
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

vector <unsigned char*> VectorPalabras; 

int indice[26]; // indice para cada letra del alfabeto

int busqueda_binaria( vector<unsigned char*>& VectorPalabras,unsigned char* palabra,int inicio, int fin){
   // busqueda binaria para buscar palabra en 
   while (inicio <= fin){
      int medio = inicio + (fin - inicio) / 2;
      //strcmp compara la palabra de medio con la palabra buscada y devuelve un numero,
      //0 si son iguales, un neg si es menor, y un pos si es mayor.
      int comparacion= strcmp((char*)VectorPalabras[medio], (char*)palabra);
      
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


void añadir_vector( vector<unsigned char*>& VectorPalabras,unsigned char* palabra, int* indice){
   /*
   esta funcion recibe el vector de palabras, una palabra nueva, y el indice con las posiciones de las letras
   tiene que insetrar la palabra de manera que se mantenga el orden, y actualizar el indice si es necesario.
   Si supera el maximo del vector, llamar a funcion para crear la copia mas grande (aun no existe).
   Vector ya crece automaticamente???
   */



}


void cargarDiccionario(const string& nombreArchivo, vector<unsigned char*>& VectorPalabras,int indice) {
    ifstream archivo(nombreArchivo);
    string linea;
     while (getline(archivo, linea)) {
        if (linea.empty()) continue;

        // 1. Reservar memoria exacta para la palabra (incluyendo el terminador \0)
        unsigned char* nuevaClave = new unsigned char[linea.length() + 1];

        // 2. Copiar el contenido de la linea al puntero uchar*
        strcpy(reinterpret_cast<char*>(nuevaClave), linea.c_str());



        ordenar_lexico(Palabras,nuevaClave,indice);


     }}



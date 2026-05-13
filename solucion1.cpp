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


void añadir_vector( vector<unsigned char*>& vectorPalabras,unsigned char* palabra, int* indice){
   /*
   esta funcion recibe el vector de palabras, una palabra nueva, y el indice con las posiciones de las letras
   tiene que insetrar la palabra de manera que se mantenga el orden, y actualizar el indice si es necesario.
   Si supera el maximo del vector, llamar a funcion para crear la copia mas grande (aun no existe).
   Vector ya crece automaticamente???
   */
   int posicion = vectorPalabras.size(); 
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

   while(posicion < vectorPalabras.size() &&
         strcmp((char*)vectorPalabras[posicion], (char*)palabra) < 0){
         posicion++; //While para  buscar el lugar para poner la palabra
         }

   vectorPalabras.insert(vectorPalabras.begin() + posicion, palabra); // Insertar la palabra en posición
   
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


void cargarDiccionario(const string& nombreArchivo, vector<unsigned char*>& VectorPalabras,int indice) {
   ifstream archivo(nombreArchivo);

   //da mensaje de error si el archivo no se pudo abrir
   if (!archivo.is_open()) {
      cout << "Error al abrir el archivo: " << nombreArchivo << endl;
      return;
     }
   char temp[100]; // temp para guardar la palabra, max 100 caracteres
       while(archivo.getline(temp, 256)){

        // reservar memoria para la palabra (temp+1 por el caracter null)
        unsigned char* palabra =
            new unsigned char[strlen(temp) + 1];

        // copiar palabra
        strcpy((char*)palabra, temp);

        // meter la palabra al vector
        añadir_vector(
            vectorPalabras,
            palabra,
            indice
        );
    }

    archivo.close();

}
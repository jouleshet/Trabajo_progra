#include <string>
#include <iostream>
using namespace std;
#include <cstdlib>
#include <vector>
#include <fstream>
#include <cstring>
#include <chrono> // <-- Librería para medir tiempos de ejecución

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


void cargarDiccionario(const string& nombreArchivo, vector<unsigned char*>& VectorPalabras, int* indice) {
   ifstream archivo(nombreArchivo);

   //da mensaje de error si el archivo no se pudo abrir
   if (!archivo.is_open()) {
      cout << "Error al abrir el archivo: " << nombreArchivo << endl;
      return;
   }

   // --- INICIO DE LA MEDICIÓN DE TIEMPO ---
   auto inicio = chrono::high_resolution_clock::now();

   char temp[100]; // temp para guardar la palabra, max 100 caracteres
   while(archivo.getline(temp, 256)){

      // Reservar memoria para la palabra (temp+1 por el caracter null)
      unsigned char* palabra = new unsigned char[strlen(temp) + 1];

      // copiar palabra
      strcpy((char*)palabra, temp);

      // meter la palabra al vector
      añadir_vector(vectorPalabras, palabra, indice);
   }

   archivo.close();

   // --- FIN DE LA MEDICIÓN DE TIEMPO ---
   auto fin = chrono::high_resolution_clock::now();
   
   // Calcular la duración en microsegundos (puedes cambiarlo a milliseconds si prefieres)
   auto duracion = chrono::duration_cast<chrono::microseconds>(fin - inicio).count();

   cout << "Tiempo de construccion/carga: " << duracion << " microsegundos." << endl;
}

// Un main simple de prueba para inicializar el índice y ejecutar la función
int main() {
   // Inicializar el índice con -1 antes de usarlo
   for(int i = 0; i < 26; i++) {
      indice[i] = -1;
   }

   // Llamamos a la función con el archivo de prueba D1.txt
   cargarDiccionario("D1.txt", VectorPalabras, indice); [cite: 76]

   // Liberar memoria de los punteros guardados al finalizar
   for(auto p : VectorPalabras) {
      delete[] p;
   }

   return 0;
}

#include <string>
#include <iostream>
using namespace std;
#include <cstdlib>
#include <vector>

typedef unsigned char uchar;

#define OVERHEAD 50
// Aqui, definimos los datos que contiene nuestro diccionario de punteros.
class Diccionario_Dinamico{
    private:
    uchar** guardador;
    int capacidad_maxima;
    int cantidad_actual;
    int Indice_Letras[28];
// Aqui se rellena el arreglo inicial
    public:
    Diccionario_Dinamico(){
        capacidad_maxima= OVERHEAD;
        cantidad_actual=0;
        guardador= new uchar*[capacidad_maxima];

        for (int i=0;i<28;i++){
            Indice_Letras[i]=-1;
        }
    }
    ~Diccionario_Dinamico(){
        delete[] guardador;
    }
    int buscar(const uchar* palabra){

        uchar Inicial= palabra[0];
        int l = Indice_Letras[Inicial];

        int tope= cantidad_actual-1;

        for (int i= Inicial+1;i<28;i++){
            if (Indice_Letras[i]!=-1){
                tope= Indice_Letras[i]-1;
                break;
            }
        }

        


        
    
    
    }
}




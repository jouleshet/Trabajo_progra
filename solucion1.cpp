#include <string>
#include <iostream>
using namespace std;
#include <cstdlib>
#include <vector>
#include <fstream>
#include <cstring>

vector <unsigned char*> Palabras;

int indice[28];


void ordenar_lexico( vector<unsigned char*>& Palabras,unsigned char*, int indice);

void cargarDiccionario(const string& nombreArchivo, vector<unsigned char*>& Palabras,int indice) {
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



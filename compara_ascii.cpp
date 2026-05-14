#include <string>
#include <iostream>
using namespace std;
#include <cstdlib>
#include <vector>
#include <fstream>
#include <cstring>

// Retorna: 
// < 0 si s1 va antes que s2
// > 0 si s1 va después que s2
// 0 si son iguales

int compararLexicografico(const unsigned char* Palabra1, const unsigned char* Palabra2) {
    int i = 0;
    //Este while, compara las palabras hasta que lleguen al simbolo
    // del espacio(/0), que significa que termina alguna de las 2 palabras
    while (Palabra1[i] != '\0' && Palabra2[i] != '\0') {
        if (Palabra1[i] < Palabra2[i]) return -1;
        if (Palabra1[i] > Palabra2[i]) return 1;
        i++;
    }
    // Si una palabra es prefijo(como se menciona en el archivo) este bloque
    // te dice cual de los 2 vale mas
    if (Palabra1[i] == '\0' && Palabra2[i] != '\0') return -1;
    if (Palabra1[i] != '\0' && Palabra2[i] == '\0') return 1;
    
    return 0;
}
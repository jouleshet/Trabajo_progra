# Trabajo_progra
Este proyecto contiene la implementación en C++ de dos estructuras de datos distintas para almacenar, buscar y gestionar un diccionario de palabras.
El objetivo es realizar un benchmarking experimental midiendo los tiempos de ejecución de operaciones básicas (Construcción, Búsqueda, Inserción y Eliminación) para contrastar su eficiencia.
solucion1.cpp: Arreglo dinámico de punteros a caracteres, el cual mantiene el orden lexicográfico de las palabras e implementa un índice alfabético auxiliar para acotar los rangos de la búsqueda binaria.
solucion2.cpp: Grilla multinivel (Skip List) basada en listas doblemente enlazadas, la cual genera subniveles o "atajos" cada $k$ nodos para optimizar la búsqueda secuencial.

en el Make: 
Para compilar ambas soluciones de manera simultánea, abra su terminal en la carpeta del proyecto y ejecute: make (si no funciona, usar mingw32-make) y para limpiar: make clean

INSTRUCCIONES DE EJECUCION:
1. Solucion 1 (Arreglo Dinamico)
El ejecutable requiere que se le pase por parametro la cantidad de palabras al azar que se desean insertar desde el diccionario para el benchmark.
Uso: .\solucion1.exe <cantidad_de_palabras>

2. Solución 2 (Grilla Multinivel)
El ejecutable requiere que se le pase el archivo base de carga y el parámetro k (tamaño de salto para los atajos). La pauta sugiere probar con k = 8, 32, 128 o 512.
Uso: .\solucion2.exe <archivo_diccionario> <valor_k>


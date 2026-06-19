# Trabajo_progra
Este proyecto contiene la implementación en C++ de dos estructuras de datos distintas para almacenar, buscar y gestionar un diccionario de palabras.
El objetivo es realizar un benchmarking experimental midiendo los tiempos de ejecución de operaciones básicas (Construcción, Búsqueda, Inserción y Eliminación) para contrastar su eficiencia.
solucion1.cpp: Arreglo dinámico de punteros a caracteres, el cual mantiene el orden lexicográfico de las palabras e implementa un índice alfabético auxiliar para acotar los rangos de la búsqueda binaria.
solucion2.cpp: Grilla multinivel (Skip List) basada en listas doblemente enlazadas, la cual genera subniveles o "atajos" cada $k$ nodos para optimizar la búsqueda secuencial.

en el Make: 
Para compilar las soluciones de manera simultánea, abra su terminal en la carpeta del proyecto y ejecute: make (si no funciona, usar mingw32-make) y para limpiar: make clean



INSTRUCCIONES DE EJECUCION:

1. Solucion 1 (Arreglo Dinamico)

Forma de ejecucion: ./solucion1.exe <Numero_experimento> <valor_N>
Ejecucion predeterminado(con valores predeterminados): ./solucion1.exe 0 0 0 0 



2. Solución 2 (Grilla Multinivel)

El ejecutable requiere que se le pase el archivo base de carga y el parámetro k (tamaño de salto para los atajos). La pauta sugiere probar con k = 8, 32, 128 o 512.

Forma de ejecucion: ./solucion2 <archivo_D1> <Valor_N> <valor_K> <Numero_experimento>

3. Solucion 3 (Arbol K-ario)

Forma de ejecucion: ./solucion3 <experimento> <valor_n> <valor_k>




Experimentos:

Experimento 0: como los tres de la seccion "Experimentacion por aplicar" utilizan parametros ya preestablecidos se juntaron en este Experimento. En particular, son tres experimentos:
    
    1:
    busqueda de  10.000 palabras de D2, entregando tiempo y numero de palabras encontradas
     
    2:
    Insercion de las primeras 5.000 palabras de D2 (despues de haber sido desordenado), entregando tiempo total y numero de incersiones exitosas (En teoria, el unico caso donde la insercion puede fallar es si la palabra ya esta en la estructura)

    3:
     Eliminacion de las 5.000 palabras restantes de D2, entregando el tiempo de ejecucion y la cantidad de eliminaciones exitosas



Experimento 1: Crear la estructura con N palabras aleatorias de D1, entregando el tiempo total de ejecucion y el espacio usado en memoria

Experimento 2: Busqueda de N palabras aleatorias en de D1 en la estructura. Devolviendo el tiempo total de busqueda y el tiempo promedio. En este experimento la estructura tiene TODAS las palabras de D1.

Experimento 3: Insercion y eliminacion intercalada de las 10.000 palabras de D2 en la estructura. Entregando el tiempo total de ejecucion y el numero de inserciones y eliminaciones exitosas.
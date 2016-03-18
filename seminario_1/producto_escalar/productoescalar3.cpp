/*
 ============================================================================
 Name        : productoescalar3.cpp
 Author      : Marlene Vásquez
 Version     :
 Copyright   : GNU Open Source and Free license
 Description : Tutorial 4. Producto escalar entre vectores.
		      Modificar la solucion 2 para que cada proceso inicialice por su
		      cuenta su parte correspondiente del vector B de forma local, de
 		      tal forma que no haya necesidad de inicializar todo el vector B
		      en el proceso 0 y repartir sus bloques entre los procesos. 
 ============================================================================
 */
#include "mpi.h"
#include <vector>
#include <cstdlib>
#include <iostream>
using namespace std;

int siguienteMultiplo(int divisor, int numero) {
    int i = 1, resultado = divisor;
    while (numero > resultado) {
        resultado = divisor * i;
        i++;
    }
    return resultado;
}

int main(int argc, char *argv[]) 
{
    int rank, size, tama;
 
    MPI_Init(&argc, &argv); // Inicializamos la comunicacion de los procesos
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Obtenemos el número total de procesos
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Obtenemos el valor de nuestro identificador

    if (argc < 2) {
        if (rank == 0) { // Solo este proceso  muetsra el mensaje
            cout << "No se ha especificado numero de elementos, por defecto sera " << size * 100;
            cout << "\n Uso: <ejecutable> <cantidad>" << endl;
        }
        tama = size * 100; // tamaño múltiplo de número de procesadores
    } else {
        tama = atoi(argv[1]);
        if (tama < size) { // El tamaño introducido es menor al número de procesos --> Tamaño = número de procesos
            tama = size;
            if (rank == 0) { // Solo este proceso muestra el mensaje
                cout << "Cantidad cambiada a " << tama << endl;
            }
        } else if (tama % size != 0) {
            tama = siguienteMultiplo(size, tama);
            if (rank == 0) { // Solo este proceso muestra el mensaje
                cout << "Cantidad cambiada a " << tama << endl;
            }
        }
    }
 
    // Creacion y relleno de los vectores
    vector<long> VectorA, VectorLocalA, VectorLocalB;
    VectorA.resize(tama, 0);
    VectorLocalA.resize(tama / size, 0);
    VectorLocalB.resize(tama / size, 0);

    if (rank == 0) { // inicializa el vector
        for (long i = 0; i < tama; i++) {
            VectorA[i] = i + 1; // Vector A recibe valores 1, 2, 3, ..., tama
        }
    }

    // Cada proceso inicializa su vector B 
    for (long i = 0, j = tama / size * rank; i < tama / size; i++, j++) {
        VectorLocalB[i] = (j + 1) * 10; // Vector B recibe valores 10, 20, 30, ..., tama*10
    }

    // Repartir VectorA
    MPI_Scatter(&VectorA[0] // referencia al vector de elementos a enviar
               ,tama / size // tamaño del vector a enviar
               ,MPI_LONG // tipo de dato que envia
               ,&VectorLocalA[0] // referencia al vector donde se almacenarán los datos recibidos
               ,tama / size // tamaño del vector a recibir
               ,MPI_LONG // tipo de dato que recibe
               ,0 // rango del proceso raiz
               ,MPI_COMM_WORLD); // Comunicador por el que se realiza la acción
 
    // Calculo de la multiplicacion escalar entre vectores
    long total, producto_local = 0;
    for (long i = 0; i < tama / size; i++) {
        producto_local += VectorLocalA[i] * VectorLocalB[i];
    }

    // El proceso 0 recolecta y suma todas las sumas locales
    MPI_Reduce(&producto_local // referencia al vector de elementos a enviar
              ,&total // referencia del al vector donde se almacena el resultado
              ,1 // tamaño del vector a enviar
              ,MPI_LONG // tipo de dato que envia
              ,MPI_SUM // operación de reducción
              ,0 // rango del proceso raiz
              ,MPI_COMM_WORLD); // Comunicador por el que se recibe

    if (rank == 0) {
        cout << "Total = " << total << endl;
    }

    MPI_Finalize();
    return 0;
}

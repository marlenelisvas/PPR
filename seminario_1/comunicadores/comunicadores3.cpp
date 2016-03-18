/*
 ============================================================================
 Name        : comunicadores3.cpp
 Author      : Marlene Vásquez
 Version     :
 Copyright   : GNU Open Source and Free license
 Description : Comunicadores 
               Modificar la solucion anterior (comunicadores2) para que tambien se 
               realice un Scatter de un vector de enteros desde el proceso 1 del 
               comunicador global a todos los procesos impares de dicho comunicador. 
               Los valores de este vector se escogen arbitrariamente en el proceso 0 
               (ojo, en el proceso con rango 0 del comunicador de rangos impares que
               es el proceso 1 de MPI Comm World), pero su tamaño debe ser igual numero
               de procesos impares en el comunicador global. El reparto asignaraá 
               un elemento de dicho vector a cada proceso impar del comunicador global.
               Se recomienda usar el comunicador de impares para realizar esta tarea.
 ============================================================================
 */
 
#include "mpi.h"
#include <vector>
#include <cstdlib>
#include <iostream>
 
using namespace std;

int main(int argc, char *argv[]) {
    int rank, size,a,b ;
    int size_par, size_impar, size_inverso , rank_par, rank_impar,rank_inverso, dato =0;

    MPI_Init(&argc, &argv); //iniciamos el entorno MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); //obtenemos el identificador del proceso
    MPI_Comm_size(MPI_COMM_WORLD, &size); //obtenemos el numero de procesos


    MPI_Comm comm_pares, // nuevo comunicador para pares
        comm_impares, // nuevo comunicador para  impares
        comm_inverso; // nuevo para todos los procesos pero con rank inverso.

   vector<int> _vector_;

   
    int color = rank %2 ;

    // creamos un nuevo cominicador
    MPI_Comm_split(MPI_COMM_WORLD // a partir del comunicador global.
            , color // los del mismo color entraran en el mismo comunicador
            // lo pares tiene color 0 y los impares 1.
            , rank, // indica el orden de asignacion de rango dentro de los nuevos comunicadores
            &comm_pares); // Referencia al nuevo comunicador creado.

     // creamos un nuevo cominicador
    MPI_Comm_split(MPI_COMM_WORLD // a partir del comunicador global.
            , color // los del mismo color entraran en el mismo comunicador
            // lo pares tiene color 0 y los impares 1.
            , rank, // indica el orden de asignacion de rango dentro de los nuevos comunicadores
            &comm_impares); // Referencia al nuevo comunicador creado.

    // creamos un nuevo comunicador inverso.
    MPI_Comm_split(MPI_COMM_WORLD, // a partir del comunicador global.
            0 // el color es el mismo para todos.
            , -rank // el orden de asignacion para el nuevo rango es el inverso al actual.
            , &comm_inverso); // Referencia al nuevo comunicador creado.

    MPI_Comm_size(comm_pares, &size_par); // Obtenemos el número total de procesos en el comunicador par
    MPI_Comm_rank(comm_pares, &rank_par); // Obtenemos el valor de nuestro identificador en el comunicador par
    
    MPI_Comm_size(comm_impares, &size_impar); // Obtenemos el número total de procesos en el comunicador impar
    MPI_Comm_rank(comm_impares, &rank_impar); // Obtenemos el valor de nuestro identificador en el comunicador impar

    MPI_Comm_size(comm_inverso, &size_inverso); // Obtenemos el número total de procesos en el comunicador inverso
    MPI_Comm_rank(comm_inverso, &rank_inverso); // Obtenemos el valor de nuestro identificador en el comunicador inverso

    

    _vector_.resize(size_impar, 0);

     if(rank == 0){
        a = 2000;
        b = 1;
    }else if (rank == 1) {// El proceso 1 (0 en el impar) inicializa el vector
                for (int i = 0; i < size_impar; i++) {
                    _vector_[i] = (i+1) * 5;
            }
    }


     //Probamos a enviar datos por distintos comunicadores
    MPI_Bcast(&b, 1, MPI_INT,
            size - 1, // el proceso con rango 0 dentro de MPI_COMM_WORLD sera root
            comm_inverso);

    if (color==0) // Sólo para los pares
        MPI_Bcast(&a, 1, MPI_INT,
            0, // el proceso con rango 0 dentro de comm sera root
            comm_pares);
    else {
            a=0;
            MPI_Bcast(&a, 1, MPI_INT,
            0, // el proceso con rango 0 dentro de comm sera root
            comm_impares);
        }


    // Repartir vector
    MPI_Scatter(&_vector_[0] // referencia al vector de elementos a enviar
               ,1 // tamaño del vector a enviar
               ,MPI_INT // tipo de dato que envia
               ,&dato // referencia al vector donde se almacenarán los datos recibidos
               ,1 // tamaño del vector a recibir
               ,MPI_INT // tipo de dato que recibe
               ,0 // rango del proceso raiz
               ,comm_impares); // Comunicador por el que se realiza la acción


    if (rank %2 == 0)
    {
        cout << "Soy el proceso " << rank <<  " he recibido el valor de a = "<< a 
             <<" y el valor de b = "<< b << endl;       
    }else{
         cout << "Soy el proceso " << rank << " he recibido el dato: " << dato << endl;
    }

    
   

    MPI_Finalize();
    return 0;


    
}

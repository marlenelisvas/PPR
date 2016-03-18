/*
 ============================================================================
 Name        : comunicadores2.cpp
 Author      : Marlene Vásquez
 Version     :
 Copyright   : GNU Open Source and Free license
 Description : Comunicadores 
               Crea 3 comunicadores nuevos (MPI_Comm_split):
               Una vez obtenidos los comunicadores, el proceso 0 (del comunicador global)
               tendrá inicialmente variables (que denominaremos 'a' y 'b') 
               que valdrán 2000 y 1 respectivamente. El resto de procesos tendrán esas dos 
               variables inicializadas con el valor 0. El proceso 0 (en el comunicador global) 
               difundirá el valor 'a' por el comunicador asociado a los procesos pares, con lo 
               que el grupo de impares no debería recibir el nuevo valor de 'a'. 
               Respecto al valor de 'b', el proceso 0 (en el comunicador global) deberá difundirlo 
               por el tercer comunicador (el de rangos inversos) y todos los procesos deberían 
               recibirlo.
 ============================================================================
 */
 
#include "mpi.h"
#include <vector>
#include <cstdlib>
#include <iostream>
 
using namespace std;

int main(int argc, char *argv[]) {
    int rank, size ,a,b;
   

    MPI_Init(&argc, &argv); //iniciamos el entorno MPI
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); //obtenemos el identificador del proceso
    MPI_Comm_size(MPI_COMM_WORLD, &size); //obtenemos el numero de procesos

    MPI_Comm comm_pares, // nuevo comunicador para pares
    	comm_impares, // nuevo comunicador para  impares
    	comm_inverso; // nuevo para todos los procesos pero con rank inverso.

    if(rank == 0 ){
    	a= 2000;
    	b= 1;
    }else{
    	a=0;
    	b=0;
    }

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

    cout << "Soy el proceso " << rank <<  " he recibido el valor de a = "<< a 
    <<" y el valor de b = "<< b << endl;


    MPI_Finalize();
    return 0;


    
}

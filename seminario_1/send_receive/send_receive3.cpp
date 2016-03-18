/*
 ============================================================================
 Name        : send_receive1.cpp
 Author      : Marlene Vásquez
 Version     :
 Copyright   : GNU Open Source and Free license
 Description : Tutorial 2. Paso de mensajes. 
                modificar la solucion para que el proceso 0 difunda su identificador de proceso (0) 
                al resto de procesos con identificadores pares, siguiendo un anillo de procesos pares, 
                y el proceso 1 haga lo mismo con los procesos impares.
 ============================================================================
 */
#include "mpi.h"
#include <iostream>
using namespace std;
 
int main(int argc, char *argv[])
{
    int rank, contador;
    MPI_Status estado;
 
    int size;

    MPI_Init(&argc, &argv); // Inicializamos la comunicacion de los procesos
    MPI_Comm_size(MPI_COMM_WORLD, &size);// 
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Obtenemos el valor de nuestro identificador

    if (rank == 0  || rank == 1){
    //Envia y recibe mensajes
    MPI_Send(&rank //referencia al vector de elementos a enviar
            ,1 // tamaño del vector a enviar
            ,MPI_INT // Tipo de dato que envias
            ,rank+2// pid del proceso destino
            ,0 //etiqueta
            ,MPI_COMM_WORLD);
}else{
        MPI_Recv(&contador // Referencia al vector donde se almacenara lo recibido
                ,1 // tamaño del vector a recibir
                ,MPI_INT // Tipo de dato que recibe
                ,rank-2// pid del proceso origen de la que se recibe
                ,0 // etiqueta
                ,MPI_COMM_WORLD // Comunicador por el que se recibe
                ,&estado); // estructura informativa del estado    

        cout<< "Soy el proceso "<<rank<<" y he recibido "<<contador<<endl;
        contador = contador+2;
        if(rank < size-2)    
           MPI_Send(&contador,1,MPI_INT, rank+2, 0, MPI_COMM_WORLD)  ; 
    }
 
    MPI_Finalize();
    return 0;
}

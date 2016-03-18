/*
 ============================================================================
 Name        : pi3.cpp
 Author      : Marlene Vásquez
 Version     :
 Copyright   : GNU Open Souce and Free license
 Description : Tutorial 3. Aproximacion de PI. 
 				Modificacion del programa solucion del calculo paralelo del numero π para
				que los subintervalos de trabajo sean distribuidos por bloques en lugar 
				de ciclicamente entre los procesos.
 ============================================================================
 */

#include <math.h>
#include <cstdlib> // Incluido para el uso de atoi
#include <iostream>
#include "mpi.h"
using namespace std;
 
int main(int argc, char *argv[]) 
{       
	int n,size, rank , size_B;
	double h = 1.0 / (double) n;
	double sum = 0.0;
	double pi_local, pi;
    double PI25DT = 3.141592653589793238462643;

	MPI_Init(&argc, &argv); // Inicializamos la comunicacion de los procesos
	MPI_Comm_size(MPI_COMM_WORLD, &size);// Obtenemos el numero de procesos
	MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Obtenemos el valor de nuestro identificador

			
	if(argc < 2){	
		
		if (rank == 0)// solo este proceso muestra el mensaje
			cout << "uso: mpirun  -np <nº proceso>  pi1  <nº de bloques>"  << endl;
			
		MPI_Finalize();		
		exit(0);
	}

	n = atoi (argv[1]);	

	MPI_Bcast(	&n,  // Puntero al dato que vamos a enviar
				1,   // Numero de datos a los que apunta el puntero
				MPI_INT,  // Tipo del dato a enviar
				0,  // Identificacion del proceso que envia el dato
				MPI_COMM_WORLD);// Comunicador por el que se recibe

	size_B = ceil( (double) n / size); // tamaño del bloque
	if(rank == 0) // solo este proceso muestra el mensaje
		cout << " Nº de bloques: " <<  size_B << endl;
	// Calculo de PI
	h = 1.0 / (double) n;
	sum = 0.0;
	pi_local = 0, pi = 0;

	for (int i = (rank*size_B) + 1; (i <= (rank + 1)* size_B) && ( i <= n ); i++) {
		double x = h * ((double)i - 0.5);
		sum+= (4.0 / (1.0 + x*x));
	}
	pi_local = h * sum ;

	cout <<" proceso "<< rank << " valor que aporta  : " << pi_local << endl;

	MPI_Reduce(&pi_local, //valor local de PI
					&pi , // Dato sobre el que vamos a reducir el resto
					1 ,  // Numero de datos que vamos a reducir
					MPI_DOUBLE, // Tipo de dato que vamos a reducir
					MPI_SUM, // Operacion que aplicaremos
					0, // proceso que va a recibir el dato reducido
					 MPI_COMM_WORLD );// Comunicador por el que se recibe

	//cout <<  "El valor aproximado de pi:" << pi << endl;			
	MPI_Bcast(&pi,1, MPI_DOUBLE, 0, MPI_COMM_WORLD);
	//cout <<  "El valor aproximado de pi:" << pi << endl;			
	if (rank == 0)//solo este proceso muestra el mensaje
		cout << "El valor aproximado de PI es: " << pi
		<< ", con un error de " << fabs(pi - PI25DT) << endl;
					
	MPI_Finalize();
	return 0;
	
} 

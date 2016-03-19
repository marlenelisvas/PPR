/*
 ============================================================================
 Name        : floyd.cpp
 Author      : Marlene Vásquez
 Version     :
 Copyright   : GNU Open Source and Free license
 Description : Algoritmo de FLOYD con reparto unidimensional (MPI)
============================================================================
*/
#include "mpi.h"
#include <iostream>
#include <fstream>
#include <string.h>
#include <vector>
#include "Graph.h"
#include <math.h>  

using namespace std;

//**************************************************************************
int main (int argc, char *argv[])
{
    int rank,size;

    Graph G;
    int nverts ;
    int tam_comp, tam_Bloque;

    int *ptr_matriz;
    int *matriz_local;
    int *vector_aux_K;

    MPI_Init(&argc, &argv); // Inicializamos la comunicacion de los procesos
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Obtenemos el número total de procesos
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Obtenemos el valor de nuestro identificador    

    if (argc != 2) 
    {
       cerr << "Sintaxis: " << argv[0] << " <archivo de grafo>" << endl;
      return(-1);
    }
    // Solo el proceso 0 realiza la lectura
    if(rank == 0)
    {
      G.lee(argv[1]);   // Read the Graph
      nverts = G.vertices; //se obtiene el número de vértices.

      // if (nverts < 100)
      // {
        //cout << "EL Grafo de entrada es:"<<endl;
        //G.imprime();
     // }
    }
  //============================================================================
  //realiza el reparto del nº de vertices a todo los procesos 
  //============================================================================
  MPI_Bcast(&nverts, 1, MPI_INT, 0, MPI_COMM_WORLD);
  //obtenemos el puntero a la matriz 
  G.getMatriz(ptr_matriz);
  //============================================================================
  // reserva de memoria para la fila y el bloque 
  // considerando que nverts % size = 0
  //============================================================================
  tam_comp = nverts * (nverts / size);
  tam_Bloque = nverts /size; 

  vector_aux_K = new int[nverts];
  matriz_local = new int [tam_Bloque * nverts];
  //============================================================================
  // Repartimos una fila por cada proceso, es posible hacer la reparticion de esta
  // manera ya que la matriz esta creada como un unico vector.
  //============================================================================
  MPI_Scatter(ptr_matriz, // Matriz que vamos a compartir
              tam_comp, // Numero de datos a compartir
              MPI_INT, // Tipo de dato a enviar
              matriz_local, // Vector en el que almacenar los datos
              tam_comp, // Numero de columnas a compartir
              MPI_INT, // Tipo de dato a recibir
              0, // Proceso raiz que envia los datos
              MPI_COMM_WORLD); // Comunicador utilizado (En este caso, el global) 
  //============================================================================
  //  Inicializacion - operaciones (fuera del bucle del algoritmo)     
  //============================================================================
  int i, j, k, vikj, root, local;
  int iG, iInit , iEnd;

  iInit = rank *tam_Bloque;
  iEnd = (rank +1)*tam_Bloque;

  MPI_Barrier(MPI_COMM_WORLD);//  Espera a todos los procesos 
  double t = MPI::Wtime(); // Se obtiene el tiempo de inicio
  //============================================================================
  //          BUCLE DEL ALGORITMO
  //============================================================================
    for(k = 0; k < nverts; k++)
    {
      root = k / tam_Bloque;
      local = k % tam_Bloque;
    
      if (k >= iInit && k < iEnd)
      for(int jL = 0; jL < nverts; jL++)
        vector_aux_K[jL] = matriz_local[local * nverts +jL ];        
      //***********  Compartiendo  k   ********************      
      MPI_Bcast(  vector_aux_K,//vector que se comparte
                  nverts, // numero de datos
                  MPI_INT,//tipo de dato
                  root,// proceso raiz
                  MPI_COMM_WORLD); //comunicador global

        for (i = 0; i < tam_Bloque; i++) {
          iG = iInit + i; 
          for (j = 0; j < nverts; j++)   
             if (iG != j && iG != k && j != k)        
                matriz_local[i * nverts + j] = min( matriz_local[i* nverts + j], matriz_local[i * nverts + k] + vector_aux_K[j]);                    
        }       
    }

    t = MPI::Wtime()-t; // se calcula el tiempo de finalizacion

  //============================================================================
  // Para imprimir los resultados -> recogemos los datos
  //============================================================================
    MPI_Gather( matriz_local, // matriz donde se han guardado los  datos a enviar
                tam_comp,// numero de datos a enviar
                MPI_INT,//tipo de dato a enviar
                ptr_matriz,// matriz donde se recogera los datos
                tam_comp,// numero de datos a  recibir
                MPI_INT,//tipo de dato a recibir
                0, // proceso raiz 
                MPI_COMM_WORLD);// comunicador utilizado (En este caso, el  global)
    MPI::Finalize();
  //============================================================================
  //  Imprime el grafo 
  //============================================================================
   if (rank == 0)//solo lo realiza el proceso 0
   {
      // if (nverts < 100)
      // {
      //   cout << endl<<"EL Grafo con las distancias de los caminos más cortos es:"<<endl<<endl;
      //   G.imprime();
      // }        
      cout<<t<<endl;
   }
    return (0);
}

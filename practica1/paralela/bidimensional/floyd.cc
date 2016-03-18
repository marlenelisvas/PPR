/*
 ============================================================================
 Name        : floyd.cpp
 Author      : Marlene Vásquez
 Version     :
 Copyright   : GNU Open Source and Free license
 Description : Algoritmo de FLOYD con reparto unidimensional (MPI)
============================================================================
*/
#include <iostream>
#include <stdlib.h> 
#include <fstream>
#include <string.h>
#include "Graph.h"
#include "mpi.h"
#include <math.h>  

using namespace std;
//*****************************************************************************
int main (int argc, char *argv[])
{
    int raiz_P, size, rank; 
   
    int tam, nverts, posicion, nverts_2;    
     
    int fila_P, columna_P, comienzo;
    Graph G; // Grafo
    int *matriz_I; 

    MPI_Comm comm_col;//comunicador 
    MPI_Comm comm_fil;//comunicador

    MPI_Init(&argc, &argv); // Inicializamos la comunicacion de los procesos
    MPI_Comm_size(MPI_COMM_WORLD, &size); // Obtenemos el número total de procesos
    MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Obtenemos el valor de nuestro identificador

    if (argc != 2) 
    {
        cerr << "Sintaxis: " << argv[0] << " <archivo de grafo>" << endl;
        MPI_Finalize();
        return(-1);
    }
    // Solo el proceso 0 realiza la lectura
    if(rank == 0)
    {
      G.lee(argv[1]);   // Read the Graph
      nverts = G.vertices; //se obtiene el número de vértices.
      if (nverts < 100)
      {
       // cout << "EL Grafo de entrada es:"<<endl;
        //G.imprime();
      }
    }
  //============================================================================
  //realiza el reparto del nº de vertices a todo los procesos 
  //============================================================================
    MPI_Bcast(&nverts,
              1,
              MPI_INT,
              0,
              MPI_COMM_WORLD);
//===================================================================
    raiz_P = sqrt(size);
    tam = nverts / raiz_P; 
    nverts_2 = nverts * nverts;
    
    int size_col = tam, 
        size_fil = tam;
    int fil_x_col = size_fil * size_col; // tam * tam
//==================================================================

    MPI_Datatype MPI_BLOQUE;
    /* Creo buffer de envio para almacenar los datos empaquetados*/
    int *buf_envio;
    buf_envio = new int [nverts_2];  //reserva de memoria  
    if(rank == 0){     
      /*Obtiene la matriz local a repartir*/
      G.getMatriz(matriz_I);///****
      /* Defino el tipo bloque cuadrado*/
      MPI_Type_vector(size_fil , size_col, nverts , MPI_INT, &MPI_BLOQUE);
      /*Creo el nuevo tipo*/
      MPI_Type_commit(&MPI_BLOQUE);     
      /*Empaqueta bloque a bloque en el buffer de envio*/
      for (int i = 0, posicion=0 ;i< size; i++)
      {      
        /* Calculo la posicion  de comienzo de cada submatriz*/
        fila_P = i / raiz_P ;
        columna_P = i % raiz_P; 
        comienzo = ( columna_P * size_col ) + ( fila_P * fil_x_col * raiz_P );

        MPI_Pack( matriz_I + comienzo,//inicio del buffer de entrada
                  1, // numero de datos de entrada
                  MPI_BLOQUE, // tipo de datos de entrada
                  buf_envio, // inicio del buffer de salida
                  sizeof(int)* nverts_2, //tamaño del buffer de salida en bytes
                  &posicion, // posicion actual del buffer
                  MPI_COMM_WORLD);//comunicador
      }
      /*libero el tipo bloque*/
      MPI_Type_free(&MPI_BLOQUE);
    }    
    /*creo un buffer de recepcion*/
    int *buf_recep;
    buf_recep = new int[fil_x_col]; 
    /*Distribuimos la matriz entre los procesos*/
    MPI_Scatter(  buf_envio, // inicio del buffer de salida
                  sizeof(int)*fil_x_col, // numero de elementos que se envia en bytes
                  MPI_PACKED, // tipo de dato que se va a enviar
                  buf_recep, // direccion del bufer de recepcion
                  fil_x_col, // numero de elementos que espera recibir
                  MPI_INT, // tipo de dato a recibir
                  0, // rank procesos raiz
                  MPI_COMM_WORLD);// comunicador
  //==========================================================================    
    int color_fil, color_col, rank_fil, rank_col;
    color_fil = rank / raiz_P;
    color_col = rank % raiz_P;  

    // creamos un nuevo comunicador
    MPI_Comm_split(MPI_COMM_WORLD,// a partir del comunicador global.
            // los del mismo color entraran en el mismo comunicador
            color_col, // 
            rank, // indica el orden de asignacion de rango dentro de los nuevos comunicadores
            &comm_col); // Referencia al nuevo comunicador creado.
    // creamos un nuevo comunicador
    MPI_Comm_split(MPI_COMM_WORLD ,// a partir del comunicador global.
            color_fil, // los del mismo color entraran en el mismo comunicador
            rank, // indica el orden de asignacion de rango dentro de los nuevos comunicadores
            &comm_fil); // Referencia al nuevo comunicador creado.

    MPI_Comm_rank(comm_col, &rank_col); // Obtenemos el valor de nuestro identificador en el comunicador col_comm    
    MPI_Comm_rank(comm_fil, &rank_fil); // Obtenemos el valor de nuestro identificador en el comunicador fil_comm
   //==========================================================================    
    int *col, *fil;
    col = new int[size_col];
    fil = new int[size_fil];   

    int i, j, k, root, local ;
    int iG, jG, iInit, iEnd, jInit, jEnd; 

    iInit = color_fil * size_fil;
    iEnd= (color_fil + 1) * size_fil; 
    jInit = color_col * size_col; 
    jEnd = (color_col + 1) * size_col; 
 
    MPI_Barrier(MPI_COMM_WORLD);//  Espera a todos los procesos 
  //=========================================================================
  //          BUCLE DEL ALGORITMO
  //=========================================================================    
    double t = MPI::Wtime(); // Se obtiene el tiempo de inicio
   
      for(k=0 ; k < nverts; k++){

        root = k / size_col;
        local = k % size_fil;
        //------copiar fila----------
        if (k >= iInit && k < iEnd)
          for(int jL = 0; jL < size_fil; jL++)
            fil[jL]= buf_recep[local * size_col + jL]; 
              
        //-------copiar columna--------- 
        if (k >= jInit && k < jEnd)       
          for(int jL = 0; jL < size_col; jL++)
            col[jL]= buf_recep[ jL* size_fil + local ];    
      //=======================================================
        MPI_Bcast( fil, size_fil, MPI_INT, root, comm_col ); 
        MPI_Bcast( col, size_col, MPI_INT, root, comm_fil );
      //=======================================================
        for (i=0 ; i < size_fil; i++){
          iG = iInit + i;
          for (j= 0; j < size_col; j++)  {   
           jG = jInit + j;
           if (iG != jG && iG != k && jG != k) 
              buf_recep[ i*tam +j ] = min(buf_recep[ i*tam+j ], col[i] + fil[j]);
          }
        }
    }      
    t = MPI_Wtime() - t; 
//============================================================================
//obtener el resultado
  MPI_Gather(   buf_recep, // matriz donde se han guardado los  datos 
                fil_x_col,// numero de datos a enviar
                MPI_INT,//tipo de dato a enviar
                buf_envio,// matriz donde se recogera los datos
                sizeof(int)*fil_x_col,// numero de datos a  recibir en bytes
                MPI_PACKED, //tipo de dato a recibir
                0, // proceso raiz 
                MPI_COMM_WORLD);// comunicador utilizado (En este caso, el  global)

  if(rank == 0)
  {
    /* Defino el tipo bloque cuadrado*/
    MPI_Type_vector(size_fil , size_col, nverts , MPI_INT, &MPI_BLOQUE);
      /*Creo el nuevo tipo*/
    MPI_Type_commit(&MPI_BLOQUE);
    
      for (int ii = 0, posicion = 0; ii< size; ii++)
      {           
          /* Calculo la posicion  de comienzo de cada submatriz*/
          fila_P = ii / raiz_P;
          columna_P = ii % raiz_P;
          comienzo = (columna_P * size_col) + (fila_P * fil_x_col * raiz_P);
                        
          MPI_Unpack( buf_envio,// inicio del buffer de entrada
                      sizeof(int)*nverts_2, //tamaño del buffer de entrada en bytes
                      &posicion,// posicion actual del buffer         
                      matriz_I + comienzo,// inicio del bufer de salida
                      1, // numero de elementos de datos de salida
                      MPI_BLOQUE,//tipo de dato
                    MPI_COMM_WORLD);//comunicador        
      }      
    /*libero el tipo bloque*/
    MPI_Type_free(&MPI_BLOQUE);
  }

  MPI::Finalize();
  if (rank == 0)//solo lo realiza el proceso 0
  {
      if (nverts < 100)
      {
       //cout << endl<<"EL Grafo con las distancias de los caminos más cortos es:"<<endl<<endl;
       // G.imprime();
      }        
       cout<< nverts<<" \n "<< t <<endl<<endl;
   }
    return (0);
}


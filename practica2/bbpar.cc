/* ******************************************************************** */
/*               Algoritmo Branch-And-Bound Secuencial                  */
/* ******************************************************************** */
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <mpi.h>
#include "libbb.h"
// #include "funciones.h"

using namespace std;
using namespace MPI;

int rank, size;
unsigned int NCIUDADES;
extern MPI_Comm comunicadorCarga, comunicadorCota; 

main (int argc, char **argv) {

  MPI_Init(&argc, &argv); // Inicializamos la comunicacion de los procesos
  MPI_Comm_size(MPI_COMM_WORLD, &size); // Obtenemos el número total de procesos
  MPI_Comm_rank(MPI_COMM_WORLD, &rank); // Obtenemos el valor de nuestro identificador

  switch (argc) {
    case 3:   NCIUDADES = atoi(argv[1]);
          break;
    default:  cerr << "La sintaxis es: bbseq <tamaño> <archivo>" << endl;
          exit(1);
          break;
  }

  int** tsp0 = reservarMatrizCuadrada(NCIUDADES);
  tNodo nodo,         // nodo a explorar
        lnodo,        // hijo izquierdo
        rnodo,        // hijo derecho
        solucion;     // mejor solucion

  bool  fin = false,        // condicion de fin
        nueva_U,      // hay nuevo valor de c.s.
        activo;
  extern bool token_presente;  // Indica si el proceso posee el token
  extern int anterior ,// Identificador del anterior proceso
             siguiente; // Identificador del siguiente proceso

  int iteraciones = 0, acotar=0;
  tPila pila;         // pila de nodos a explorar
  int  U;// valor de c.s.
  U = INFINITO;                  // inicializa cota superior

  int nodosExplorados = 0; 


    //comunicadores
  MPI_Comm_dup(MPI_COMM_WORLD, &comunicadorCarga);
  MPI_Comm_dup(MPI_COMM_WORLD, &comunicadorCota);

   // Determinamos anterior y siguiente.
  anterior = (rank - 1 + size) % size;  // inicializa proceso anterior
  siguiente = (rank + 1) % size;        // inicializa proceso siguiente
//------------------------------------------------------------------------------
  if(rank == 0){
     token_presente = true;
    //Leer_Problema_Inicial(&nodo);  
    LeerMatriz (argv[2], tsp0);    // lee matriz de fichero  
    activo = Inconsistente(tsp0);

   if(!activo) InicNodo (&nodo);              // inicializa estructura nodo
   
    //El proceso 0 reparte la matriz tps0
    MPI_Bcast(&tsp0[0][0], // Puntero al dato que vamos a enviar
                  NCIUDADES*NCIUDADES,  // Numero de datos a los que apunta el puntero
                  MPI_INT, // Tipo del dato a enviar
                  0, // Identificacion del proceso que envia el dato
                  MPI_COMM_WORLD); 

  }else{
    token_presente = false;
    //El proceso 0 reparte la matriz tps0
    MPI_Bcast(&tsp0[0][0], // Puntero al dato que vamos a enviar
                  NCIUDADES*NCIUDADES,  // Numero de datos a los que apunta el puntero
                  MPI_INT, // Tipo del dato a enviar
                  0, // Identificacion del proceso que envia el dato
                  MPI_COMM_WORLD); 

    Equilibrado_Carga(&pila, &fin,&solucion);
    if(!fin) pila.pop(nodo);
  }

  double t=MPI::Wtime();
  //===============================================================
  //-----------------------ciclo del Branch&Bound------------------  
  //===============================================================
  while(!fin){          
    
        //Ramifica(&nodo, &lnodo, &rnodo);
        Ramifica(&nodo, &lnodo, &rnodo, tsp0);
        nueva_U = false;       

        if(Solucion(&rnodo)){
          if(rnodo.ci() < U) { 
            U = rnodo.ci(); //Actualiza cota sup
            nueva_U = true;
            CopiaNodo (&lnodo, &solucion);
          }
        }else{  // Si no es un nodo hoja
          if(rnodo.ci() < U) if(!pila.push(rnodo)){
            printf ("Error: pila agotada 1\n");
            liberarMatriz(tsp0);
            exit (1);
          }
        }
     
        if(Solucion(&lnodo)){

          if(lnodo.ci() < U){
            U = lnodo.ci();// Actualiza cota sup
            nueva_U = true;
            CopiaNodo (&lnodo, &solucion);

          }  
        }else{ // No es un nodo hoja
          if(lnodo.ci()  < U) if( !pila.push(lnodo)){
            printf ("Error: pila agotada 2 \n");
            liberarMatriz(tsp0);
            exit (1);
          }
        }
     
       // hay_nueva_cota_superior = Difusion_Cota_Superior(&U);
        nueva_U = Difusion_Cota_Superior(&U,nueva_U);

        // if(hay_nueva_cota_superior) Acotar(&pila, U);
        if (nueva_U){   pila.acotar(U);   acotar ++;  }  
      
        Equilibrado_Carga(&pila, &fin,&solucion);

        if(!fin)  pila.pop(nodo);

        iteraciones++;
      
    }
  //----------------------------------------------------------------

  t=MPI::Wtime()-t;

  if (rank == 0){
    printf ("Solucion: \n");
    EscribeNodo(&solucion);
    cout<< "Tiempo gastado= "<<t<<endl;
    cout << "Numero de iteraciones = " << iteraciones << endl << endl;

    //cout<< t<<"\t" << iteraciones <<"\t" << endl;
  } 

  liberarMatriz(tsp0);
  MPI_Finalize();

  exit(0);
}










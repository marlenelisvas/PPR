/* ******************************************************************** */
/*               Algoritmo Branch-And-Bound Paralelo                    */
/* ******************************************************************** */
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <mpi.h>
#include "libbb.h"

using namespace std;

extern MPI_Comm comunicadorCarga, comunicadorCota;  
extern int  anterior, siguiente; 
extern bool token_presente;

unsigned int NCIUDADES;
int rank, size;
bool retorno;
/* ******************************************************************** */

int main (int argc, char **argv) {
  MPI_Init(&argc, &argv);
  MPI_Comm_size(MPI_COMM_WORLD, &size);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);

  switch (argc) {
    case 3:
      NCIUDADES = atoi(argv[1]);
      break;
    default:
      if (rank == 0)
        cerr << "La sintaxis es: bbseq <tamaño> <archivo>" << endl;
      MPI_Finalize();
      exit(-1);
    break;
  }

  int**   tsp0 = reservarMatrizCuadrada(NCIUDADES);

  tNodo   *nodo = new tNodo(),   // nodo a explorar
          *rnodo = new tNodo(),  // hijo izquierdo
          *lnodo = new tNodo(),  // hijo derecho
          *solucion = new tNodo();  // mejor solucion

  bool    fin = false,              // condicion de fin
          nueva_U;                  // hay nuevo valor de c.s.
  int     U,                        // valor de c.s.
          iteraciones = 0,
          acotaciones = 0;

  tPila   *pila = new tPila();      // pila de nodos a explorar

  U = INFINITO;                         // inicializa cota superior
  anterior = (rank - 1 + size) % size;  // inicializa proceso anterior
  siguiente = (rank + 1) % size;        // inicializa proceso siguiente

  MPI_Comm_dup(MPI_COMM_WORLD, &comunicadorCarga);
  MPI_Comm_dup(MPI_COMM_WORLD, &comunicadorCota);



  if (rank == 0) {
    token_presente = true;
    LeerMatriz(argv[2], tsp0);

    MPI_Bcast(&tsp0[0][0], NCIUDADES * NCIUDADES, MPI_INT, 0, MPI_COMM_WORLD);
    InicNodo(nodo);                // inicializa estructura nodo

  } else {
    token_presente = false;
    MPI_Bcast(&tsp0[0][0], NCIUDADES * NCIUDADES, MPI_INT, 0, MPI_COMM_WORLD);
    Equilibrado_Carga(pila, &fin, solucion);

    if (!fin) pila->pop(*nodo);
  }

  double t = MPI_Wtime();
  //-------------------ciclo de Branch&Bound---------------------
  while (!fin) { 
    Ramifica(nodo, rnodo, lnodo, tsp0);
    nueva_U = false;
    retorno = false;

    if (Solucion(lnodo)) {
      if (lnodo->ci() < U) {
        U = lnodo->ci(); // actualiza cota sup
        nueva_U = true;
        CopiaNodo(lnodo, solucion);
      }
    } else { // si no es nodo hoja
      if (lnodo->ci() < U) 
        if(!pila->push(*lnodo)){
          printf ("Error: pila agotada\n");
          liberarMatriz(tsp0);
          exit (1);
        }      
    }

    if (Solucion(rnodo)) {
      if (rnodo->ci() < U) {
        U = rnodo->ci(); // actualiza cota sup
        nueva_U = true;
        CopiaNodo(rnodo, solucion);
      }
    } else { // no es nodo hoja
      if (rnodo->ci() < U) 
        if(!pila->push(*rnodo)){
          printf ("Error: pila agotada\n");
          liberarMatriz(tsp0);
          exit (1);
        }      
    }

    Difusion_Cota_Superior(&U, &nueva_U);
   
    if (nueva_U)  pila->acotar(U);   

    Equilibrado_Carga(pila, &fin, solucion);

    if (!fin)  pila->pop(*nodo);

    iteraciones++;
  }
  //------------------------------------------------------
  t = MPI_Wtime() - t;


  if (rank == 0) {
     cout << "Solución = " << endl;
	     EscribeNodo(solucion);
     cout << "Tiempo gastado= " << t << endl;     
    //cout<< endl << t << "\t"<< rank << ":" << iteraciones << "\t"; //
  }else 
     // cout << rank << ":" << iteraciones << "\t";
 
  liberarMatriz(tsp0);
  delete pila;
  delete nodo;
  delete rnodo;
  delete lnodo;
  delete solucion;

  MPI_Finalize();

  exit(0);
}

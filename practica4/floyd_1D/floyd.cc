#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <string.h>
#include <omp.h>
#include "Graph.h"



using namespace std;

int main(int argc, char *argv[]) {
  int num_threads, chunk, nverts, i, j, k, ik, kj, ij, *B;
  double t;

  if (argc != 3){
      std::cerr << "Sintaxis: " << argv[0] << " <archivo de grafo> <num_threads>" << std::endl;
      return(-1);
    }else{
      num_threads = atoi(argv[2]);
    }

    omp_set_num_threads(num_threads);

    Graph G;
    G.lee(argv[1]); // Lee el grafo

   // cout << "El grafo de entrada es:" << endl;
   // G.imprime();
   

    nverts = G.vertices;                                              // Número de vértices
    num_threads > nverts ? chunk = 1 : chunk = nverts / num_threads;  // Tamaño de bloque

    int *A = G.getMatriz();//obtenemos la matriz
    B = (int *) malloc(nverts * nverts * sizeof(int));    // Se reserva espacio 
    G.swap(A,B);                                          // copiamos
  //===================================================================================
    t = omp_get_wtime();//inicio-tiempo
    int filK[nverts];
    #pragma omp parallel private (i, j, ik, ij, kj,filK)
    { 
    for (k = 0; k < nverts; k++) {
      //hacer privada la fila k para que este en la cache
      #pragma omp single copyprivate(filK)
      {
        for(i = 0; i<nverts; i++)
          filK[i]= B[k*nverts+i];
      }
       #pragma omp for schedule(static, chunk)   // inicio de la región paralela, 
                                                                                  //reparto estático por bloques
      for (i = 0; i < nverts; i++) {
        ik = i * nverts + k;
        for (j = 0; j < nverts; j++) {
          if (i != j && i != k && j != k) {
            kj = k * nverts + j;
            ij = i * nverts + j;
            B[ij] = min(B[ik] + filK[j], B[ij]);
          }
        }//fin for
      }//fin for

    }
  }
    t = omp_get_wtime() - t;//fin-tiempo
  //==================================================================================
    G.swap(B,A);  // Se copia en el grafo 


    //cout << endl << "El grafo con las distancias de los caminos más cortos es:" << endl;
    G.imprime();
    //cout << "Tiempo gastado = " << t << endl << endl;
  
    cout << t << endl;
  

    delete[] B;

    return(0);
}
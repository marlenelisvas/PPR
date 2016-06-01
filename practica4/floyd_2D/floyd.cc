#include <iostream>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <string.h>
#include <omp.h>
#include "Graph.h"
#include <math.h>  


using namespace std;

int main(int argc, char *argv[]) {
  int num_threads, chunk, nverts, tam, *B;
  int raiz_P;
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
     
      //cout << "El grafo de entrada es:" << endl;
      //G.imprime();


      raiz_P = sqrt(num_threads);
      nverts = G.vertices;          // Nº de vértices
      tam = nverts / raiz_P;  // Tamaño de bloque

      int *A = G.getMatriz();
      B = (int *) malloc(nverts * nverts * sizeof(int));    // Se reserva espacio 
      G.swap(A,B);                                        // Se copia la matriz 
      int colK[nverts], filK[nverts];

      int i, j, k,ij,id;
      int kG, iG, iInit, iEnd, jInit, jEnd,fila_P,columna_P; 
      t = omp_get_wtime();

      #pragma omp parallel private(id, i, j, k, ij, iInit, iEnd, jInit, jEnd, fila_P, columna_P, kG, iG)// inicio de la región paralela
      { 
        id = omp_get_thread_num();
        fila_P = id / raiz_P;
        columna_P = id % raiz_P;

        iInit = fila_P * tam;
        iEnd = (fila_P + 1) * tam;

        jInit = columna_P * tam;
        jEnd = (columna_P + 1) * tam;

        for (k = 0; k < nverts; k++) {

          #pragma omp barrier
          if (k >= iInit && k < iEnd){  // La fila K pertenece al proceso
             kG = k * nverts;   
             for (i = 0; i < nverts; i++)                  
              filK[i] = B[kG + i];            
            }

          if (k >= jInit && k < jEnd) // La columna K pertenece al proceso
            for (i = 0; i < nverts; i++) 
              colK[i] = B[nverts * i + k];            
          
          #pragma omp barrier

          for (i = iInit; i < iEnd; i++) {
            iG = i * nverts;
            for (j = jInit; j < jEnd; j++) 
              if (i != j && i != k && j != k) {
                ij = iG + j;
                B[ij] = min(colK[i] + filK[j], B[ij]);
              }            
          }

        }
      }

    t = omp_get_wtime() - t;//fin-tiempo
  //==================================================================================
    G.swap(B,A);  // Se copia en el grafo 


    //cout << endl << "El grafo con las distancias de los caminos más cortos es:" << endl;
    //G.imprime();
    //cout << "Tiempo gastado = " << t << endl << endl;
  
    cout << t << endl;
  

    delete[] B;

    return(0);
}
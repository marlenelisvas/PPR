#include <iostream>
using namespace std;

#define TAGCARGA 0
#define TAGCOTA 1



enum ESTADO {  PASIVO,  ACTIVO,  TERMINAR};// Estado del proceso
enum TIPOMENSAJE {  PETICION,  NODOS,  TOKEN,  FIN};// Tipos de mensajes
enum COLOR {  BLANCO,  NEGRO };// Colores que pueden tener tanto los procesos como el token

#ifndef MENSAJE_COTA_H
#define MENSAJE_COTA_H
struct MensajeCota {
  int origen;
  int cota;
};
#endif

#ifndef MENSAJE_H
#define MENSAJE_H
struct Mensaje {
  int tipo;
  int *nodos;
  int numNodos;
  int colorToken;
  int origen;
};
#endif

int color_token = NO;
int MICOLOR = BLANCO;

bool difundir_cs_local = false; // Indica si el proceso puede difundir su cota inferior local
bool pendiente_retorno_cs = false;  // Indica si el proceso est� esperando a recibir la cota inferior de otro proceso


/* ******************************************************************** */
/* ********************EQUILIBRADO DE CARGA**************************** */
/* ******************************************************************** */
void Equilibrado_Carga(tPila *pila, int *estadoAct , int rank, int size , MPI_Comm comunicadorCarga ){

  Mensaje mensaje, msjTmp;
  tNodo nodo;

  MPI_Status status;
  int hayMensajes, solicitante, tam;
  int *vdatos;

  tPila pila_N;

  if(pila->vacia()){   // el proceso no tiene trabajo, pide a otros procesos
    cout << "proceso " <<rank<<endl;
      mensaje.tipo = PETICION;
      mensaje.origen = rank;
      //Enviar peticion de trabajo al proceso (id+1)%P;
      MPI_Send(&mensaje, sizeof(Mensaje), MPI_BYTE, (rank + size) % size,TAGCARGA, comunicadorCarga);
      *estadoAct = PASIVO;
      
  }
   //Esperar mensaje de otro proceso
    
    MPI_Iprobe(MPI_ANY_SOURCE, TAGCARGA, comunicadorCarga, &hayMensajes, &status);
     //MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, comunicadorCarga, &status); 

    while( (pila->vacia()|| hayMensajes) && *estadoAct != TERMINAR){
        
      //Esperar mensaje de otro proceso    
      MPI_Probe(MPI_ANY_SOURCE, TAGCARGA, comunicadorCarga, &status);      
      //Recibir mensaje           
      MPI_Recv(&mensaje, sizeof(Mensaje), MPI_BYTE, status.MPI_SOURCE, TAGCARGA, comunicadorCarga, &status);
    cout << "proceso  while " <<rank<<" status "<< status.MPI_SOURCE<< " mensaje tipo "<<mensaje.tipo <<endl;
      switch(mensaje.tipo){

        case PETICION:  // Peticion de trabajo

              if(pila->vacia()){

                if(mensaje.origen == rank){ // Peticion devuelta

                  *estadoAct = PASIVO;//detectar posible fin
                  //volver a enviar el mensaje
                  MPI_Send(&mensaje, sizeof(Mensaje), MPI_BYTE, (rank + 1) % size,TAGCARGA, comunicadorCarga);
                  cout << "proceso  SWITCH " <<rank<<" status "<< status.MPI_SOURCE<< " mensaje tipo "<<mensaje.tipo <<endl;
                  if(color_token != NO){
                    if(rank == 0) color_token = BLANCO;
                    else if (MICOLOR == NEGRO) color_token = NEGRO;

                    mensaje.tipo = TOKEN;
                    mensaje.colorToken = color_token;
                    MPI_Send(&mensaje,sizeof(Mensaje), MPI_BYTE, (rank+1)%size, TAGCARGA,comunicadorCarga);

                    MICOLOR = BLANCO;
                    color_token = NO;
                  }
                }else  //Reenviar peticion 
                   MPI_Send(&mensaje, sizeof(Mensaje), MPI_BYTE, (rank+1)%size, TAGCARGA, comunicadorCarga);                               

              }else if( pila->tamanio()>1){   //hay trabajo para ceder 

               //divide la pila
                pila->divide(pila_N);
                msjTmp.numNodos = pila_N.tamanio();
                msjTmp.nodos = &pila_N.nodos[0];
                msjTmp.tipo = NODOS;

                //Se los enviamos a mensaje.solicitante 
                MPI_Send(&msjTmp, sizeof(Mensaje), MPI_BYTE,mensaje.origen, TAGCARGA, comunicadorCarga);

                if(rank < mensaje.origen)     MICOLOR = NEGRO;

              }else
                  MPI_Send(&mensaje, sizeof(Mensaje), MPI_BYTE, (rank+1)%size, TAGCARGA, comunicadorCarga); 

              // //Recibir mensaje de peticion de trabajo;
              // MPI_Recv(&solicitante,1,MPI_INT,MPI_ANY_SOURCE, P ,comunicadorCarga,&estado);
              // //Reenviar peticion de trabajo al proceso (id+1)%P;
              // MPI_Send(&solicitante, 1, MPI_INT, (id + 1) % P, PETICION, comunicadorCarga);

              // if(solicitante == id){  // Peticion devuelta
              //   MPI_Send(&solicitante, 1, MPI_INT, (id + 1) % P, PETICION, comunicadorCarga);
              //  // Iniciar deteccion de posible situacion de fin;
              // }
          break;
        case NODOS:   // Resultado de una peticion de trabajo
            MPI_Get_count(&status, MPI_INT, &tam);
            //Recibir nodos del proceso donante;
            MPI_Recv(&mensaje, sizeof(Mensaje), MPI_BYTE, status.MPI_SOURCE, TAGCARGA, comunicadorCarga, &status);
            nodo = pila->nodos[pila->tope];
            nodo[0]= mensaje.nodo;
            pila->push(nodo);              
            //Almacenar nodos recibidos en la pila;
            pila->tope = tam;
            break;

        case TOKEN:

                  color_token = mensaje.colorToken;

                   cout << "proceso  SWITCH " <<rank<<" status "<< status.MPI_SOURCE<< " mensaje tipo "<<mensaje.tipo <<endl;
                  if(*estadoAct == PASIVO){

                    if(rank == 0 && MICOLOR == BLANCO && color_token == BLANCO){
                      MPI_Iprobe(MPI_ANY_SOURCE, TAGCARGA, MPI_COMM_WORLD, &hayMensajes, &status);
                      while(hayMensajes) {
                        MPI_Recv(&mensaje, sizeof(Mensaje), MPI_BYTE, status.MPI_SOURCE, TAGCARGA, MPI_COMM_WORLD, &status);
                        MPI_Iprobe(MPI_ANY_SOURCE, TAGCARGA, MPI_COMM_WORLD, &hayMensajes, &status);
                      }
                      //enviamos fin
                      mensaje.tipo = FIN;
                      MPI_Send(&mensaje, sizeof(Mensaje), MPI_BYTE,(rank+1)%size, TAGCARGA, MPI_COMM_WORLD); 
                      *estadoAct = TERMINAR;
                    }
                  }
                  else { 
                   if(rank == 0) color_token = BLANCO;
                  else if(MICOLOR == NEGRO) color_token = NEGRO;
                  
                    mensaje.tipo = TOKEN;
                    mensaje.colorToken = color_token;
                    MPI_Send(&mensaje, sizeof(Mensaje), MPI_BYTE,(rank+1)%size, TAGCARGA,comunicadorCarga);
                    MICOLOR = BLANCO;
                    color_token = NO;                 
        }

      break;
      case FIN:
            *estadoAct = TERMINAR;
        if(rank < size-1) {
          mensaje.tipo = FIN;
          MPI_Send(&mensaje, sizeof(Mensaje), MPI_BYTE,(rank+1)%size, TAGCARGA, comunicadorCarga); 
        }

      break;

      }
      hayMensajes = false;

    }
}

  //   if(!fin){ // el proceso tiene nodos para trabajar
  //   //  Sondear si hay mensajes pendientes de otros procesos;
  //     MPI_Iprobe((id - 1) % P, MPI_ANY_TAG, comunicadorCarga, &hayMensajes, &estado);
  //     while(hayMensajes){ // atiende peticiones mientras haya mensajes
  //      // Recibir mensaje de peticion de trabajo;
  //       MPI_Recv(&solicitante, 1, MPI_INT, MPI_ANY_SOURCE, PETICION, MPI_COMM_WORLD,&estado);
  //       //if(hay suficientes nodos en la pila para ceder)
  //       if (pila->tamanio() > 1) { 
  //         //divide la pila
  //         pila->divide(pila_N);
  //         //Enviar nodos al proceso solicitante;
  //         MPI_Send(&pila_N.nodos[0], pila_N.tope, MPI_INT,solicitante, NODOS, comunicadorCarga);

  //       }else
  //         //Reenviar peticion de trabajo al proceso (id+1)%P;
  //         MPI_Send(&solicitante, 1, MPI_INT, (id + 1) % P, PETICION, comunicadorCarga);
   
  //       //Sondear si hay mensajes pendientes de otros procesos;
  //       MPI_Iprobe((id - 1) % P, MPI_ANY_TAG, comunicadorCarga, &hayMensajes, &estado);
  //     }
  //   }
  // }
//======================================================================
//                EDIFUSION DE COTA SUPERIOR
//======================================================================
bool Difusion_Cota_Superior(int *U ,bool nueva_U, int rank, int size, MPI_Comm comunicadorCota){

  MPI_Status status;
  MensajeCota mensajeCota, msjTemp;

  int hayMensajes;
  int cotaRecibida;
 
  bool retorno = nueva_U;

  if(difundir_cs_local && !pendiente_retorno_cs){
    //Enviar valor local de cs al proceso (id+1)%P;
    mensajeCota.cota= *U;
    mensajeCota.origen = rank;
    MPI_Send(&mensajeCota, 1, MPI_INT, (rank + 1) % size, TAGCOTA, comunicadorCota);
    pendiente_retorno_cs = true;
    difundir_cs_local = false;
  }
  
  //Sondear si hay mensajes de cota superior pendientes;
  MPI_Iprobe((rank-1+size) % size, TAGCOTA, comunicadorCota, &hayMensajes, &status);

  while(hayMensajes>0){   

    //Recibir mensaje con valor de cota superior desde el proceso (id-1+P)%P;
    MPI_Recv(&msjTemp,sizeof(MensajeCota), MPI_BYTE, (rank-1+size)% size , TAGCOTA, comunicadorCota, &status);
    //Actualizar valor local de cota superior; 
    if(msjTemp.cota < *U){
         *U = msjTemp.cota;   
         retorno = true;
    } 

    if(msjTemp.origen == rank && difundir_cs_local){      //Enviar valor local de cs al proceso (id+1) % P;           
      mensajeCota.origen = rank;
      mensajeCota.cota = *U;

      MPI_Send(&mensajeCota, sizeof(Mensaje), MPI_BYTE, (rank + 1) % size, TAGCOTA, comunicadorCota);
      
      pendiente_retorno_cs = true;
      difundir_cs_local = false;

    }else if(msjTemp.origen == rank && !difundir_cs_local)
      pendiente_retorno_cs = false;
    else// origen mensaje == otro proceso
      //Reenviar mensaje al proceso (id+1)%P;
      MPI_Send(&msjTemp, sizeof(Mensaje), MPI_BYTE, (rank + 1) % size, TAGCOTA, comunicadorCota);
    
    //Sondear si hay mensajes de cota superior pendientes;
    MPI_Iprobe((rank-1 + size) % size, TAGCOTA, comunicadorCota, &hayMensajes, &status);
  }
  return retorno;
}



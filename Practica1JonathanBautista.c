#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

// constantes de etiqueta.
#define TAG_MENSAJE 1
#define TAG_MENSAJES 2
void inicializa(int a){
	time_t t;
	srand(time(&t)+a);
}
// Jonathan Bautista Parra 
int main(int argc, char** argv)
{
   
	int size;
	int rank;
    int traidor;//Entero que representa el nodo traidor.
    int numTraidores; 


	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
     inicializa(rank);
                int esTraidor=(rand()%(5+2*rank))%2;//se elige un traidor aleatoriamente.
                if(esTraidor==1){
                    printf("El nodo %i es traidor\n",rank);
                }else{
                    printf("El nodo %i es leal\n",rank);
                }
                 

                char mensaje;//arreglo que contiene los mensajes originales de cada nodo.
                char mensajesT[size];//arreglo que contiene los mensajes alterados de cada nodo dados por el nodo traidor.
                char posiblesM[2]={'A','R'};
                    mensaje=posiblesM[rand()%2]; // se asigna el mensaje original de cada nodo
                                                //de manera aleatoria
                
                
                char mensajesParaRank [size];//arreglo con el mensajes que envía cada nodo (distinto del el rank) en la primer ronda
                char mensajesOtrosRank[size][size];//areglo de los mensajes reportados por los otros nodos (segunda ronda)
                int contadorA[size]; //condador de las veces que el nodo (i) fue reportado con mensaje 'A'
                int contadorR[size]; //condador de las veces que el nodo (i) fue reportado con mensaje 'R'
                //inicializamos los arreglos con puros 0's.
                for(int i=0;i<size;i++){
                    contadorA[i]=0;
                    contadorR[i]=0;
                }
                int mayoria[size];// arreglo con contiene el plan real de cada nodo.
                for(int r = 0;r<size;r++){// cada nodo envía su mensaje a los demás nodos.(primer ronda)
                    if(rank!=r){
                        MPI_Send(&mensaje, 1, MPI_CHAR, r, TAG_MENSAJE, MPI_COMM_WORLD);
                    }
                }
                   for(int r = 0;r<size;r++){// cada nodo recibe el mensaje de los demás nodos.(primer ronda)
                    if(rank!=r){
                        MPI_Recv(&mensajesParaRank[r], 1, MPI_INT, r, TAG_MENSAJE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                        
                            if(mensajesParaRank[r]=='A'){
                            contadorA[r]++;
                            }if(mensajesParaRank[r]=='R'){
                           contadorR[r]++;
                            }
                        
                    }else{//como el nodo sabe su mensaje, ponemos una -.
                         mensajesParaRank[r]='-';
                    }                        
                }
                for(int r = 0;r<size;r++){//cada nodo envía los mensajes reportados a los demás nodos.(segunda ronda)
                    
                    if(rank!=r&& esTraidor==0){
                        
                        MPI_Send(&mensajesParaRank,size , MPI_CHAR, r, TAG_MENSAJES, MPI_COMM_WORLD);
                    }if(esTraidor==1){//si en nodo que envía es el traidor, envía otros mensajes.
                        for(int i=0;i<size;i++){//          se asignan los mensajes de cada nodo que dará el traidor
                    if(i!=rank){//               de manera aleatoria.
                         mensajesT[i]=posiblesM[rand()%2];
                    }else{
                        mensajesT[i]='-';            //el traidor no reporta su mensaje por que ya lo reportó en la primer ronda.
                    }
                }
                        MPI_Send(&mensajesT, size , MPI_CHAR, r, TAG_MENSAJES, MPI_COMM_WORLD);
                    }
                }
                   for(int r = 0;r<size;r++){//cada nodo recibe los mensajes reportados a los demás nodos.(segunda ronda)
                    if(rank!=r){
                        MPI_Recv(&mensajesOtrosRank[r], size, MPI_INT, r, TAG_MENSAJES, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                        
                         for(int i=0;i<size;i++){
                            if(mensajesOtrosRank[r][i]=='A'){
                          contadorA[i]++;
                        }if(mensajesOtrosRank[r][i]=='R'){
                           contadorR[i]++;
                        }
                         }
                        
                        
                    }else{
                         for(int i=0;i<size;i++){//como el nodo sabe sus mensaje, ponemos una -.
                                mensajesOtrosRank[r][i]='-';

                         }
                    }                        
                }

     
                for(int i=0; i<size;i++){
                    printf("nodo %i informa: ",i);
                    for(int j=0; j<size;j++){
                         printf("%c", mensajesOtrosRank[i][j]);
                    }
                    printf(" a nodo %i \n",rank);
                }
                printf("\n");
                printf("\n");
                
                printf("[");
                for(int i=0; i<size;i++){
                    
                    if(contadorA[i]>contadorR[i]){
                        mayoria[i]='A';
                    }else{
                       mayoria[i]='R';
                    }
                }
                printf("Mayoria en rank %i:\n",rank);
                printf("[");
                for(int i=0; i<size;i++){//imprimimos el plan real de cada nodo, según el nodo rank.
                    printf("%c ",mayoria[i]);
                
                }
                printf("]");
                printf("\n");
                printf("\n");
                
                
	MPI_Finalize();
	return 0;
}
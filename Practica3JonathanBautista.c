#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

/**
 * \mainpage Práctica 3
 * \author Jonathan Bautista Parra
 * \date Mayo-2023
 */

 #define TAG_ELECCION 1
#define TAG_RESPUESTA 2
#define TAG_COORDINADOR 3

void inicializa(int a){
	time_t t;
	srand(time(&t)+a);
}

int ran(int a){
	time_t t;
	srand(time(&t));
    int s = rand()%a;
    return s;
}

int main(int argc, char** argv)
{
    int size; //número de nodos
	int rank; //nodo en el programa
	int seconds; //timeout
    char eleccion = 'e';//mensaje inicial para hacer el llamado a elección. 
    char eleccionR;//mensaje recibido.
    //char respuesta[size];
    int coordinador=-1;//variable que guardará al coordinador.
    int count=0;// contador de timeouts

    

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    inicializa(rank);

    char respuestas[size];
    for(int i=0;i<size;i++){
        respuestas[i]='-';
    }
    int caido;// variable que nos indica si un nodo está caido o no. (0 si no está caido, 1 en caso contrario).
    time_t now;
    int llamado = ran(size-1); // nodo que lanza el llamado. 
    time(&now);
    struct tm *local = localtime(&now);
    seconds =  local->tm_sec;
    inicializa(rank);
    caido= rand()%2;//se eligen a los caidos de manera aleatoria.
    if(rank==llamado){//el nodo que hace el primer llamado no debe estar caido.
        caido=0;
        
    }
    if(rank==size-1){//el último nodo (de mayor número) debe estar caido. 
        caido=1;
    }
    
    if(rank==llamado){//se realiza el primer llamado a elecciones.
        eleccion='e';
        for(int i=llamado+1; i<size;i++){
            MPI_Send(&eleccion, 1 , MPI_CHAR, i, TAG_ELECCION, MPI_COMM_WORLD);
            printf("llama=%i\n",llamado);
        }
    }

    while(coordinador==-1){//ciclo que termina cuando se declara un nuevo coordinador. 
        MPI_Status estado;
        MPI_Recv(&eleccionR, 1, MPI_CHAR, MPI_ANY_SOURCE, TAG_ELECCION, MPI_COMM_WORLD, &estado);
        printf("nodo: %i recibió mensaje =%c, de nodo: %i\n",rank,eleccionR,estado.MPI_SOURCE);
        if(eleccionR=='e'){
            if(caido==1){
                char r = 't';//mensaje de simulación time out. 
                MPI_Send(&r, 1 , MPI_CHAR, estado.MPI_SOURCE, TAG_ELECCION, MPI_COMM_WORLD);
                printf("nodo %i envía mensaje t a nodo %i\n",rank,estado.MPI_SOURCE);
                
            }else{
                char r = 'o'; //mesaje de que está disponible para ser lider
                MPI_Send(&r, 1 , MPI_CHAR, estado.MPI_SOURCE, TAG_ELECCION, MPI_COMM_WORLD);
                printf("nodo %i envía mensaje o a nodo %i\n",rank,estado.MPI_SOURCE);
                for(int i=rank+1; i<size;i++){
                MPI_Send(&eleccion, 1 , MPI_CHAR, i, TAG_ELECCION, MPI_COMM_WORLD);
                printf("nodo %i envía mensaje e a nodo %i\n",rank,i);
                }
            } 
        }if(eleccionR=='t'){
            respuestas[size-1]='t';
            count=0;
            for(int i=rank+1;i<size;i++){
                if(respuestas[i]=='t'){
                    count++;
                }
            }
            printf("count= %i\n",count);

            int check=0;
            for(int i=rank+1;i<size;i++){
                check++;
            }
            if(count==check){
                int c='c';//mensaje de que es coordinador.
                for(int i=0;i<size;i++){
                    if(caido==1){
                        MPI_Send(&c, 1 , MPI_CHAR, i, TAG_ELECCION, MPI_COMM_WORLD);

                    }
                }
            }
        }if(eleccionR=='c'){
            coordinador= estado.MPI_SOURCE;
            printf("El coordinador es: %i\n",coordinador);
        }
    }
    




        
    
    
    

    	MPI_Finalize();
	return 0;
}



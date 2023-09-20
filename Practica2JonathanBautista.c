#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

/**
 * \mainpage Práctica 2
 * \author Jonathan Bautista Parra
 * \date Abril-2023
 */
// constantes de etiqueta.
#define TAG_HOUR 1
#define TAG_MINUTES 2
#define TAG_DIFERENCE 3

void inicializa(int a){
	time_t t;
	srand(time(&t)+a);
}
// Jonathan Bautista Parra 
int main(int argc, char** argv)
{
    int size; //número de nodos
	int rank; //nodo en el programa
	int hours, minutes; // variables para la hora en cada nodo.
    int hoursM; // horas del nodo maestro
    int minutesM; // minutos del nodo maestro
    int retraso;// difirencia dada al reloj de cada nodo 
    int diferencia; //diferencia entre la hora del nodo maestro y el nodo esclavo 
    


	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    inicializa(rank);
    time_t now;
    time(&now);
    struct tm *local = localtime(&now);
    retraso = -(rand()%30) + 15; //se genera un retraso aleatorio entre -15 y 15 minutos.
    hours = local->tm_hour;   //tomamos la hora actual.      
    minutes = (local->tm_min) + retraso; //tomamos los minutos actuales y le agregamos el retraso.
    if(minutes<0){ //se hace un ajuste para que no tengamos minutos negativos o mayores a 59 min.
        hours--;
        minutes=60+minutes;
    }if(minutes>59){
        hours++;
        minutes=minutes-60;
    }
    if(rank==0){ //el nodo maestro, que es el nodo 0, envía su hora (horas y minutos) a todos (incluido él)
        
        for(int i=0; i<size;i++){
            MPI_Send(&hours, 1 , MPI_INT, i, TAG_HOUR, MPI_COMM_WORLD);
            MPI_Send(&minutes, 1 , MPI_INT, i, TAG_MINUTES, MPI_COMM_WORLD);
        }
    }       
   // todos reciben la hora del nodo maestro.
    MPI_Recv(&hoursM, 1, MPI_INT, 0, TAG_HOUR, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    MPI_Recv(&minutesM, 1, MPI_INT, 0, TAG_MINUTES, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    if(hours<0){// se pone la hora en la pantalla.
        if(rank==0)printf("En el nodo %i (Maestro) son las 23:%02d horas\n",rank, minutes);
        else printf("En el nodo %i Son las 23:%02d horas\n",rank, minutes);
        

    }else{
        if(rank==0) printf("En el nodo %i (Maestro) son las %02d:%02d horas\n",rank, hours, minutes);
        else printf("En el nodo %i Son las %02d:%02d horas\n",rank, hours, minutes);

    }
    if(hours!=hoursM){// se calcula la diferencia entre el nodo maestro y los demás nodos.
        if(hoursM>hours){
            diferencia= -(60+minutesM-minutes);
        }else{
            diferencia= (60+minutes-minutesM);
        }

    }else{
        diferencia= minutes-minutesM;
    }
    MPI_Send(&diferencia, 1 , MPI_INT,0, TAG_DIFERENCE, MPI_COMM_WORLD);// se envía esa diferencia al nodo maestro.
    
   
   if(rank==0){// el nodo maestro recibe las diferencias, calcula el promedio y manda el ajuste a todos los nodos (incluido él)
    int diferencias[size];
    int promedio;
    for(int i=0; i<size;i++){
          MPI_Recv(&diferencias[i], 1, MPI_INT, i, TAG_DIFERENCE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
          diferencias[i]-=10/2; //simulamos un tiempo de llegada del mensaje de 10 minutos xd
          promedio+=diferencias[i];
        }
        promedio=promedio/size;
    for(int i=0; i<size;i++){
            MPI_Send(&promedio, 1 , MPI_INT,i, TAG_DIFERENCE, MPI_COMM_WORLD);

        }

    }
    //los  nodos reciben el ajuste y cambian sus relojes a la nueva hora.
    MPI_Recv(&retraso, 1, MPI_INT, 0, TAG_DIFERENCE, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
    printf("\n");
    /*if(diferencia<=0){
        minutes+=(retraso-diferencia);
        if(minutes>59){
        hours++;
        minutes=minutes-60;
    }if(minutes<0){
        hours--;
        minutes=60+minutes;
    }
    }else{
        minutes-=(retraso-diferencia);
        if(minutes<0){
        hours--;
        minutes=60+minutes;
    }if(minutes>59){
        hours++;
        minutes=minutes-60;
    }
    }*/
    minutes+=(retraso-diferencia);
        if(minutes>59){
        hours++;
        minutes=minutes-60;
    }if(minutes<0){
        hours--;
        minutes=60+minutes;
    }
    if(hours<0){
        if(rank==0)printf("La nueva hora en el nodo %i (Maestro) es: las 23:%02d horas\n",rank, minutes);
        else printf("La nueva hora en el nodo %i es: las 23:%02d horas\n",rank, minutes);
        

    }else{
        if(rank==0) printf("La nueva hora en el nodo %i (Maestro) es: las %02d:%02d horas\n",rank, hours, minutes);
        else printf("La nueva hora en el nodo %i es: las %02d:%02d horas\n",rank, hours, minutes);

    }
          
	MPI_Finalize();
	return 0;
}
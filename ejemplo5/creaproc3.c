#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


void alarma(int sig)
{
    printf("acabo de recibir un SIGALRM\n");
}


int main()
{
    signal(SIGALRM,alarma); //le da la instrucción al sistema operativo de que, si en algún momento
    //recibe un SIGALRM, ejecute la función alarma()
    printf("Acabo de programar la captura de un SIGALRM\n");
    alarm(3);//programa la alarma para que se dispare en 3 segundos
    printf("Ahora he programado la alarma en 3 seg.\n");
    pause();//para que el programa se qeude congelado esperando que se dispare la alarma y se ejecute la función alarma()
    //depsués, se ejecutará alarma() y, cuando ésta termine, el programa continuará con la ejecución normal
    printf("Ahora continúo con la ejecución normal\n");

    exit(0);
}

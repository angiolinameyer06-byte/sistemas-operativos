#include <signal.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>


int status,pid;

void finhijo()
{
    pid=wait(&status);//wait() devuelve el PID del hijo que ha muerto y almacena su estado de salida en la variable status
}


int main()
{
    signal(SIGCHLD,finhijo); //cuando llegue la señal SIGCHLD, ejecuta la función finhijo()

    if(fork()==0) //si fork devuelve 0, significa que estamos en el proceso hijo, que se va a suicidar
    {
        sleep(3);
        exit(5);
        //el hijo se suicida con exit(5), lo que genera la señal SIGCHLD para el proceso padre
    }

    pause();//el proceso padre se queda en pausa esperando a que llegue la señal SIGCHLD, que le indica que su hijo ha muerto
    printf("mi hijo ha muerto con estado %d\n",status/256);
    printf("ahora continúo con la ejecución\n");
    exit(0);
}

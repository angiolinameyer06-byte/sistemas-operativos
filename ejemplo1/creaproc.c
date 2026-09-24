//Angiolina

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main()
{
    pid_t pid; //variable para almacenar un número, donde se guardará el identificador de los procesos 
    pid = fork(); //se duvlipa el proceso, creando 2 iguales. Un padre y un hijo
    switch (pid)
    {
        case -1: //caso de error
        printf ("No he podido crear el proceso hijo \n");
        break;

        case 0: //identificador del hijo
        printf ("Soy el hijo, mi PID es %d y mi PPID es %d \n", getpid(), getppid());
        sleep (20);
        break;
        
        default: //identificador con número aleatorio (padre)
        printf ("Soy el padre, mi PID es %d y el PID de mi hijo es %d \n", getpid(), pid);
        sleep (30);
    }
printf ("Final de ejecución de %d \n", getpid());
//como se ejecutan en paralelo, el tiempo de sleep empezará a correr a la vez para los dos, acabando primero el hijo y, 10 segundos después, el padre
exit (0);
}
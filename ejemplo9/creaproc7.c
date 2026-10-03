#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main()
{
    switch(fork())
    {
        case -1 : /* ERROR */

            perror("Error en fork");
            exit(1);

        case 0 : /* HIJO */

            printf("Hola, soy el hijo. Espero 2 segundos...\n");
            sleep(2);
            kill(getppid(),SIGUSR1);
            printf("Soy el hijo. He señalado a mi padre. Adios.\n");
            exit(0);

        default : /* PADRE */

            printf("Hola, soy el padre y voy a esperar.\n");
            signal(SIGUSR1, SIG_IGN); /* Ignoro señal para no morir */
            //como se le dice al sistema operativo que, al llegarle SIGUSR1, no haga nada y continúe con su ejecución normal,
            //se quedará en pausa durante un tiempo indefinido, dejando el programa colgado, ya que pause() espera una señal
            //para dejar de hacer que el programa esté en pausa.

            pause();
            printf("Soy el padre y ya he recibido la señal.\n"); //esto nunca se ejecutará
            exit(0);
    }
}
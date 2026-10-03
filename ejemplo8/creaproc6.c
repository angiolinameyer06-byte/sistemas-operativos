#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int seguir = 1; /* Variable global */

/**
 * Inicialmente, el código se quedaba en un bucle infinito porque alarm nunca llegaba a ejecutarse. Entonces, he añadido
 * un sleep(1) para que se congele durante 1 segundo, alarm(5) pueda ejecutarse y enviar la señal SIGALRM al sistema operativo,
 * que a su vez ejecutará la función fin() y cambiará el valor de seguir a 0, lo que hará que el bucle termine.
 * 
 * El valor que se le ponga a sleep() determinará el números de líneas que se imprimirán.
 */

void fin(int n)
{
    seguir = 0;
}

int main()
{
    int contador = 0;

    signal(SIGALRM, fin); //le da la instrucción al sistema operativo de que, si en
    //algún momento recibe un SIGALRM, ejecute la función fin()

    alarm(5); //pone un temporizador de 5 segundos que enviará una señal SIGALRM al sistema operativo
    
    do
    {
        sleep(1); //para que el programa se quede congelado durante 1 segundo y le dé tiempo al
        //sistema operativo a recibir la señal SIGALRM

        printf("Esta es la línea %d\n", contador++);
    } while (seguir); //mientras seguir sea distinto de 0, porque en C, cualquier valor distinto de 0 se considera verdadero


    printf("TOTAL: %d líneas\n", contador);
    exit(0);

} 
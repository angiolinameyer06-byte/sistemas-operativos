#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h> //para usar el wait

int main ( )
{
    int estado, numero;

    switch(fork())
    {
        case -1 : /* ERROR */
            perror("Error en fork");
            exit(1);
        case 0 : /* HIJO */
            numero = 13;
            printf("Soy el hijo y muero con %d...\n", numero);
            sleep(20); //espera 20 segundos
            exit(numero); //muere y deja el valor de la variable numero en reposo
        default : /* PADRE */
            wait(&estado); //coge el número que ha dejado flotando exit()
            //coge la variabole estado y la rellena con la causa de la muerte del hijo

            if ((estado & 0x7F) != 0) //se queda sólo con los 7 bits menos significativos, que son los que
            // indican la causa de la muerte del hijo, y se fije si es distinto de 0, lo que indica que ha muerto con una señal
            {
                printf("Mi hijo ha muerto con una señal.\n");
            }
            else
            {
                printf("Mi hijo ha muerto con exit(%d).\n", (estado>>8) & 0xFF); //mueve los 8 bits menos significativos a la
                // derecha y se queda con los 8 bits menos significativos, que son los que indican la causa de la muerte del hijo
            }
            exit(0);
    }
}

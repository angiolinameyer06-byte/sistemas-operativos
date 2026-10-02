#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main (int argc, char *argv[])
{
    int i;

    printf ("\nEjecutando el programa invocador (prog1). Sus argumentos son: \n");

    for ( i = 0; i < argc; i ++ ) //para recorrer el vector de argumentos
    {
        printf (" argv[%d] : %s \n", i, argv[i]); //muestra cada argumento
    }
    sleep( 10 ); //para durante 10 segundos para mostrar los argumentos de prog1
    strcpy (argv[0],"prog2"); //en el vector de argumentos, cambia prog1 por prog2

    if (execvp ("./prog2", argv) < 0) //sustituye el proceso actual (prog1) por prog2, pasando el vector de argumentos.
    // si, al intentar ejecutar prog2, hay un error, muestra un mensaje de error y termina el programa con un valor de salida 1
    {
        printf ("Error en la invocacion a prog2 \n");
        exit (1);
    };
    exit (0);
} 
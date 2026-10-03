#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


int main()
{
    int df,tam;
    long numero;
    char buffer[10];//array de caracteres para almacenar la cadena leída del dispositivo tty

    df=open("/dev/tty", O_RDONLY);//abre la terminal actual para lectura, que es el terminal desde el que se ejecuta el programa
    
    if (df<0)//si no se ha podido abrir el dispositivo tty, muestra un mensaje de error y termina el programa
    {
        perror("Error al abrir tty");
        exit(-1);
    }
    tam=read(df, buffer, 9); /* COMO MUCHO DE HASTA 9 DIGITOS */ //lee hasta 9 caracteres del dispositivo tty y
    //los almacena en el buffer

    if (tam == -1)//si no se ha podido leer del dispositivo tty, muestra un mensaje de error
    {
        perror("Error de lectura");
    }
    else
    {
        buffer[tam]=0; /* PONE EL FINAL DE CADENA */ //pone un carácter nulo al final del buffer para indicar el final de la cadena
        numero=atoi(buffer);//convierte la cadena leída del buffer a un número entero
        printf("Resultado: %ld\n",numero*2);
    }
    close(df);
    exit(0);
} 
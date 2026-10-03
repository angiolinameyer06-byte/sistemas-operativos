#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>


int main(int argc, char *argv[], char *env[])
{
    int df, cont=0;
    char buffer[4096];//cambio el número de bytes para que quepan las variables largas de Linux

    df=creat("variables.txt", 0755);//crea un archivo llamado "variables.txt" con permisos 0755
    //(lectura, escritura y ejecución para el propietario, y lectura y ejecución para el grupo y otros usuarios).

    if (df<0)
    {
        perror("Error al crear archivo");
        exit(-1);
    }

    while (env[cont] != NULL) //mientras no llegue a la posición NULL, seguirá recorriendo el array de punteros del entorno
    {
        strcpy(buffer, env[cont]);//copia la variable que lea en ese momento en buffer
        strcat(buffer, "\n");//le añade un salto de línea al final

        if (write(df, buffer, strlen(buffer)) != strlen(buffer))//escribe en el archivo asociado a df, "strlen(buffer)" bytes y compara,
        //si el número de bytes que pidió escribir es distinto a los realmente escritos, lanza un error y para
        {
            perror("Error al escribir");
            break; /* NO SIGUE */
        }
        cont++;
    }
    close(df);//cierra el archivo
    exit(0);
}

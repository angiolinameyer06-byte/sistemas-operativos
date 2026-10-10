#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h> 


int main(int argc, char *argv[])
{
    //para controlar que el número de argumentos introducido es válido
    if (argc != 3)
    {
        char *msg = "Debes introducir 3 argumentos.\n";

        //el 2 es para que escriba el error por el canal de errores
        //msg es el nombre de la variable que tiene el mensaje de error a mostrar
        //número de caracteres a imprimir
        write(2, msg, 30);

        return 1;
    }

    char *archivo_origen = argv[1]; //se guarda un puntero a la cadena de texto con el nombre del archivo que se le pasa al programa
    off_t tam_chunk = atol(argv[2]); //pasa el tamaño que se le ha pasado al programa a un entero largo y se guarda en una variable
    //de un tipo especial para manejar tamaños de archivos

    if (tam_chunk <= 0)//se comprueba si el tamaño que deben tener los cachos de archivo es un número correcto
    {
        char *msg = "El tamaño del pedazo debe ser mayor a 0.\n";
        write(2, msg, 40);//para mostrar el mensaje de error
        return 1;
    }

    
    int fd_src = open(archivo_origen, O_RDONLY);//abre el archivo en modo lectura.
    //Si se ha podido abrir, devuelve un descriptor de fichero. Si ha ocurrido algun error, devuelve un número negativo
    
    if (fd_src < 0)
    {
        perror("Error al abrir el archivo de origen");
        return 1;
    }

    off_t file_size = lseek(fd_src, 0, SEEK_END);//se coloca al final del archivo para calcular el tamaño del archivo
    lseek(fd_src, 0, SEEK_SET); //vuelve al inicio del archivo

    if (file_size <= 0)//por si ha ocurrido un error
    {
        char *msg = "El archivo está vacío o error al calcular tamaño.\n";
        write(2, msg, 50);
        close(fd_src);
        return 1;
    }

    
    int num_chunks = (file_size + tam_chunk - 1) / tam_chunk;//calcula el número de fragmentos (hijos) a crear


    for (int i = 0; i < num_chunks; i++)//para crear los hijos y tuberías
    {
        int pipefd[2];

        if (pipe(pipefd) < 0)//por si hay un error
        {
            perror("Error al crear la tubería");
            exit(1);
        }

        pid_t pid = fork();//para crear el hijo
        if (pid < 0)
        {
            perror("Error en fork");
            exit(1);
        }

        if (pid == 0) //esto es lo que hará el hijo
        {
            close(pipefd[1]);//cierra el extremo de escritura de la tubería
            close(fd_src);//cierra el archivo original


            char dest_name[512];//genera el nombre del archivo correspondiente

            snprintf(dest_name, sizeof(dest_name), "%s.h%02d", archivo_origen, i);//para ponerle nombre al archivo correspondiente


            int fd_dest = open(dest_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);//crea y abre el archivo destino
            if (fd_dest < 0)
            {
                perror("Error al crear archivo de destino");
                exit(1);
            }

            char buffer[4096];//se crea una variable auxiliar
            ssize_t bytes_leidos; //variable para almacenar cuantos bytes llegaron en cada lectura
            
            while ((bytes_leidos = read(pipefd[0], buffer, sizeof(buffer))) > 0)//mientras se reciban bien los datos que se quieren escribir en el archivo que toque en esta iteración
            {
                write(fd_dest, buffer, bytes_leidos);//se escribe el contenido de la variable bytes_leidos en el archivo destino
            }

            close(pipefd[0]);//cierra su extremo de la tubería
            close(fd_dest);//cierra el archivo destino
            exit(0);//muere
        }
        else //lo que hará el padre
        {
            close(pipefd[0]); //cierra su extremo de lectura de la tubería

            //calcula cuántos bytes le toca enviar a este hijo exacto
            off_t bytesAEnviar = tam_chunk;
            if (i == num_chunks - 1) //porque el último pedazo puede ser más pequeño
            {
                bytesAEnviar = file_size - (i * tam_chunk);
            }

            //leer del archivo origen y lo vuelca a la tubería del hijo
            char buffer[4096];
            off_t enviadosTotales = 0;

            while(enviadosTotales < bytesAEnviar)
            {
                //calcula cuántos bytes faltan por enviar en este pedazo
                off_t bytesRestantes = bytesAEnviar - enviadosTotales;

                off_t por_leer;

                if (bytesRestantes < (off_t)sizeof(buffer))
                {
                    por_leer = bytesRestantes;//si queda poco, lee solo lo que falta
                } else
                {
                    por_leer = sizeof(buffer);//si queda mucho, llena el búfer entero
                }
                
                ssize_t r = read(fd_src, buffer, por_leer);//lee el archivo original y verifica que la lectura es correcta

                if (r <= 0)//si hay un error, sale del bucle
                {
                    break;
                }

                write(pipefd[1], buffer, r);//mete los datos leídos en la tubería hacia el hijo
                enviadosTotales += r;//se actualiza el contador de bytes enviados
            }

            close(pipefd[1]);//cierra escritura para que el hijo detecte el fin del archivo en la tubería
        }
    }

    //cierre del archivo original y matanza de hijos
    close(fd_src);

    for (int i = 0; i < num_chunks; i++)
    {
        wait(NULL);
    }

    return 0;
}
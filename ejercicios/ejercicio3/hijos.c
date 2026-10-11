#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/shm.h>


int main(int argc, char *argv[])
{
    //para controlar los argumentos
    if (argc != 3)
    {
        char *msg = "Debes incluir 3 argumentos.\n";
        write(2, msg, 27);
        return 1;
    }

    int x = atoi(argv[1]);
    int y = atoi(argv[2]);

    if (x <= 0 || y <= 0) //control de parámetros
    {
        char *msg = "Los parámetros x e y deben ser mayores que 0.\n";
        write(2, msg, 45);
        return 1;
    }

    pid_t pidSuperpadre = getpid(); //el pid del superpadre (el programa principal)

    //se crea un único segmento de memoria compartida para guardar todos los PID, tanto los de los hijos como los de las hojas
    int shmid = shmget(IPC_PRIVATE, (x + y) * sizeof(pid_t), IPC_CREAT | 0666);

    if (shmid < 0)//error al crear la memoria compartida
    {
        perror("Error al crear la memoria compartida");
        return 1;
    }

    
    pid_t *shm_ptr = (pid_t *)shmat(shmid, NULL, 0);//adjunta la memoria compartida al espacio de direcciones del superpadre

    if (shm_ptr == (void *)-1) //si ocurre un error al enganchar la memoria compartida al proceso
    {
        perror("Error al adjuntar la memoria compartida");
        shmctl(shmid, IPC_RMID, NULL);
        return 1;
    }

    //se divide el puntero en dos zonas dentro del mismo bloque compartido
    pid_t *ancestros = shm_ptr;//para guardar los PIDs de la cadena vertical
    pid_t *hojas = shm_ptr + x;//para guardar los PIDs de las hojas finales

    int i;

    for (i = 0; i < x; i++)//bucle para crear todos los procesos intermedios (de cantidad x)
    {
        pid_t pid = fork();

        if (pid < 0)
        {
            perror("Error en fork");
            exit(1);
        }

        if (pid == 0)//lo que ejecutará el nuevo hijo
        {
            ancestros[i] = getpid();//guardamos su PID en el array de antecesores
        }
        else//lo que hará el padre
        {
            break; //se rompe para que el padre no cree más hijos en paralelo
        }
    }

    //si i es igual a x, significa que estamos en el último eslabón de la cadena vertical
    if (i == x)
    {

        for (int j = 0; j < y; j++)//para crear los hijos hojas
        {
            pid_t pidHoja = fork();

            if (pidHoja < 0)
            {
                perror("Error en fork (hoja)");
                exit(1);
            }

            if (pidHoja == 0) //lo que hace cada proceso hoja
            {
                pid_t pidSubhijo = getpid();

                hojas[j] = pidSubhijo; //se guarda el PID de la hoja en la memoria compartida

                // Mensaje obligatorio de la parte baja mostrando sus antecesores de la cadena
                printf("Soy el subhijo %d, mi padres son: ", pidSubhijo);

                for (int k = 0; k < x; k++)//para mostrar los mensajes
                {
                    printf("%d", ancestros[k]);
                    if (k < x - 1)
                    {
                        printf(", ");
                    }
                }
                printf("\n");

                shmdt(shm_ptr);
                exit(0);
            }
        }


        for (int j = 0; j < y; j++)//el último eslabón (el último de la cadena de tamaño x) espera a que terminen todos sus hijos (los hijos hojas)
        {
            wait(NULL);
        }
    }
    else //los eslabones intermedios y el superpadre esperan a su hijo directo
    {
        wait(NULL);
    }


    //el superpadre (único que llega hasta aquí) muestra el mensaje final con los PIDs de las hojas recogidos de la memoria compartida
    if (pidSuperpadre == getpid())
    {
        printf("Soy el superpadre (%d) : mis hijos finales son: ", pidSuperpadre);
        for (int k = 0; k < y; k++)
        {
            printf("%d", hojas[k]);
            if (k < y - 1)
            {
                printf(", ");
            }
        }
        printf("\n");
    }

    //desvincular y destruir el segmento de memoria compartida
    shmdt(shm_ptr);
    shmctl(shmid, IPC_RMID, NULL);

    return 0;
}
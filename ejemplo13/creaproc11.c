#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int shmid;
int *numero=NULL;

/**
 * función que crea la memoria compartida
 */
int creaComp(void)
{
    if((shmid=shmget(IPC_PRIVATE,sizeof(int),IPC_CREAT|0666))==-1)//se crea el bloque de memoria compartida y se le asigna el identificador a shmid
    {
        perror("Error al crear memoria compartida: ");
        return 1;
    }

    /* vinculamos el segmento de memoria compartida al proceso */
    numero=(int *) shmat (shmid,0,0);//coge la dirección de memoria donde está el pedazo de memoria RAM compartida y lo guarda en el puntero numero

    /*iniciamos las variables */ 
    *numero=0;
    return 0;
} 

void borraComp(void)
{
    char error[100];

    if (numero!=NULL)
    {
        /* desvinculamos del proceso la memoria compartida */
        if (shmdt((char *)numero)<0)
        {
            sprintf(error,"Pid %d: Error al desligar la memoria compartida: ",getpid());
            perror(error);

            exit(3);
        }
        /* borramos la memoria compartida */
        if (shmctl(shmid,IPC_RMID,0)<0)
        {
            sprintf(error,"Pid %d: Error al borrar memoria compartida: ",getpid());
            perror(error);

            exit(4);
        }
        numero=NULL;
    }
} 


int main(void)
{
    pid_t pid;


    if (creaComp() != 0)//el padre crea el cacho de memoria compartida
    {
        fprintf(stderr, "Error al inicializar la memoria compartida\n");
        return 1;
    }

    printf("El padre con PID %d ha creado la memoria compartida. Valor inicial en el hueco reservado: %d\n", getpid(), *numero);

    pid = fork();//se crea al hijo

    if (pid < 0)//si el proceso no se ha podido crear, se borra la memoria compartida
    {
        perror("Error en fork");
        borraComp();
        return 1;
    }
    else if (pid == 0)//si el PID es 0, significa que estamos trabajando con el hijo
    {
        printf("El hijo con PID %d está escribiendo el número 42 en la memoria compartida\n", getpid());
        *numero = 42;
        exit(0);
    }
    else //en caso de trabajar con el padre
    {
        wait(NULL);//espera a que el hijo termine su ejecución

        printf("El hijo ya ha terminado. Leemos de la memoria el número que escribió: %d\n", *numero);

        borraComp();//se desatruye la memoria compartida
    }

    return 0;
}
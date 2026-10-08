#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<signal.h>

void manejadorAlarma(int sig)
{
    //no va a hacer nada, sólo existe para qeu Z no muera al no poder manejar la señal recibida
}

/**
 * Función apra generar el pstree
 */
void hacerPstree(int signal)
{
    char pid_str[16];
        
    sprintf(pid_str, "%d", getppid()); //se pasa a char para poder meterlo en execlp

    // se crea un hijo para que ejecute el exec y se transforme en el pstree sin destruir el proceso A
    if (fork() == 0)
    {
        execlp("pstree", "pstree", "-p", pid_str, NULL);

        //si pasa aquí es porque ha ocurrido un error
        perror("Error al ejecutar pstree");
        exit(1);
    }
    
    // El proceso A espera a que el hijo que ejecutó pstree termine de imprimir
    wait(NULL);

}

/**
 * Función que se encarga de que Z le mande la señal a A para que este ejecute el pstree
 */
void crearAlarma(pid_t pidA)
{
    kill(pidA, SIGUSR1);//le manda la señal a A
}


void crearArbol(int seg)
{

    pid_t pidA = fork();// creo el hijo de arb y guardo su pid

    if(pidA < 0)//caso de error
    {
        perror("Error al crear el proceso hijo.\n");
        exit(0);
    }
    else if(pidA == 0)//lo que ejecutará el proceso A
    {
        signal(SIGUSR1, hacerPstree);//cuando le llegue la señal que manda Z, se meterá en la función para hacer pstree

        pid_t pidA_real = getpid();//pra obtener el identificador real de A y pasarlo para mandar la señal 
        pid_t pidB = fork(); //creo el hijo de A, B

        if(pidB < 0)//caso de error
        {
            perror("Error al crear el proceso hijo.\n");
            exit(0);
        }
        else if(pidB == 0)//lo que ejecutará B
        {
            signal(SIGUSR2, manejadorAlarma); //B  se prepara apra la señal de muerte que le enviará A

            pid_t pidX = fork(); //creo el hijo X

            if(pidX < 0) //caso error
            {
                perror("Error al crear el proceso hijo.\n");
                exit(0);
            }
            else if(pidX == 0)//código que ejecutará el proceso X únicamente
            {
                pause();
                exit(0);
            }

            pid_t pidY = fork();//se crea Y

            if(pidY < 0)
            {
                perror("Error al crear el proceso hijo.\n");
                exit(0);
            }
            else if (pidY == 0)
            {
                pause();
                exit(0);
            }

            pid_t pidZ = fork();//se crea Z

            if(pidZ < 0)
            {
                perror("Error al crear el proceso hijo.\n");
                exit(0);
            }
            else if(pidZ == 0)
            {
                signal(SIGALRM, manejadorAlarma);//para que no muera por la señal SIGALRM sin manejarla

                alarm(seg);//espera los segundos que se le pasó al programa como argumento
                pause();//se duerme a Z 
                crearAlarma(pidA_real);//se llama a la función apra crear la alarma
                pause(); //congelo a Z para que salga en el pstree
                exit(0);
            }

            pause(); //B espera a que A le avise de que pstree ha acabado

            kill(pidX, SIGTERM); // mata a X
            kill(pidY, SIGTERM); // mata a Y
            kill(pidZ, SIGTERM); // mata a Z


            wait(NULL);//espera a X
            wait(NULL);//espera a Y
            wait(NULL);//espera a Z

            exit(0);

        }
        pause(); //A se queda dormido esperando la señal de Z

        kill(pidB, SIGUSR2); //A mata a B

        wait(NULL);//espera a que B termine
        exit(0);
    }            
    //esto que viene lo ejecutará arb
    wait(NULL);
}


int main(int argc, char *argv[])
{

    if (argc < 2)
    {
        printf("Se debe incluir el número de segundos de espera.\n");
        return 1;
    }

    int seg = atoi(argv[1]);//convertimos el texto ingresado en consola a un número entero

    crearArbol(seg);

    return 0;
}
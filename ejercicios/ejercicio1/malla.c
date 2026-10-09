#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>


/**
 * Función encargada de hacer el pstree que se muestra en la terminal
 */
void mostrar_pstree()
{
    char pid_str[16];//para guardar el pid del proceso en un array de char
    sprintf(pid_str, "%d", getpid());//se transforma el pid del proceso actual a tipo array de char

    if (fork() == 0)
    {
        execlp("pstree", "pstree", "-c", pid_str, NULL);//para que el proceso actual (el nuevo que se ha creado para que el padre no se convierta en el pstree) ejecute el pstree

        perror("Error al ejecutar pstree");
        exit(1);
    }

    wait(NULL);//el padre espera a que el hijo (el recién creado (el pstree)) termine 
}


/**
 * Función encargada de generar tantos procesos como valores x hayan pasado por la terminal
 */
void crear_columna(int x)
{
    for (int i = 2; i <= x; i++)
    {
        pid_t pid_row = fork();

        if (pid_row < 0)
        {
            perror("Error en fork al crear fila");
            exit(1);
        }
        else if(pid_row > 0)//para que el padre se quede esperando a qeu su hijo muera
        {
            wait(NULL);
            exit(0);
        }
        //el hijo continúa aquí
    }

    //el último proceso creado se queda parado esperando recibir la señal SIGTERM que le mandará malla
    pause();
    exit(0);
}


/**
 * Función encargada de generar tantos procesos como valores y hayan pasado por la terminal y de matar los procesos en orden
 */
void crear_malla(int x, int y)
{

    for(int j = 1; j <= y; j++)//empieza por 1 porque en el dibujo del enunciado empiezan por 1
    {
        pid_t pid_col = fork();

        if (pid_col < 0)
        {
            perror("Error en fork al crear columna");
            exit(1);
        }
        else if(pid_col == 0)//para que el hijo cree las columnas
        {
            crear_columna(x);
        }
    }

    sleep(1);//para asegurarnos de que la malla esté hecha

    mostrar_pstree();//para mostrar el árbol en la terminal

    //para matar la malla en orden
    signal(SIGTERM, SIG_IGN);
    kill(0, SIGTERM);

    while (wait(NULL) > 0);//para asegurarse de que malla no acabe antes de que mueran sus hijos
}


/**
 * Función main 
 */
int main(int argc, char *argv[])
{
    if (argc != 3)//para verificar que se introduce el número correcto de argumentos
    {
        printf("Tienes que introducir 3 argumentos.\n");
        return 1;
    }


    //se pasan a enteros para poder comparar los valores
    int x = atoi(argv[1]);
    int y = atoi(argv[2]);


    if (x <= 0 || y <= 0)//para controlar que se introduzcan números correctos
    {
        printf("Los argumentos x e y deben ser números mayores a 0.\n");
        return 1;
    }

    crear_malla(x, y);

    return 0;
}
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
 printf("Voy a suicidarme\n");

 kill(getpid(), SIGKILL);//kill() envía la señal SIGKILL al proceso getpid()

 perror("No he muerto???");//si el proceso no ha muerto, es que ha habido un error y perror() nos lo indica.
 //Si el proceso ha muerto, esta línea nunca se ejecutará

 exit(0);
} 
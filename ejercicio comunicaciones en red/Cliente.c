// Angiolina Meyer - Y2872983K

#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<arpa/inet.h>

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

const int PUERTO = 9999;


int establecer_canal()
{
    int conector = socket(AF_INET, SOCK_STREAM, 0);

    if(conector < 0)
    {
        perror("Error al crear el socket");
        exit(1);
    }

    return conector;
}

void leer_del_socket(int conector)
{
    char buffer[4096];
    ssize_t salida;

    while((salida = read(conector, buffer, sizeof(buffer))) > 0)//ha leído bytes pero no ha llegado al final
    {
        write(1, buffer, salida);
    }
    if(salida < 0)
    {
        perror("Error al leer del socket");
        exit(1);
    }

    close(conector);
    return;
}

void crear_conexion(int conector, struct sockaddr_in *dir_servidor)
{
    if(connect(conector, (struct sockaddr *)dir_servidor, sizeof(*dir_servidor)) < 0)
    {
        perror("Error al conectar con el servidor");
        exit(1);
    }

    leer_del_socket(conector);

    return;
}

int main(int argc, char *argv[])
{
    if(argc != 2)
    {
        printf("Debes ejecutar de la forma: Cliente 'IP del servidor'.\n");
        exit(1);
    }

    //se crea el struct que contendrá la infmración del servidor que se quiere conectar
    struct sockaddr_in dir_servidor = {0};

    dir_servidor.sin_family = AF_INET;//se usará el protocolo de red IPv4
    dir_servidor.sin_port = htons(PUERTO);//se asigna el puerto y se transforma al orden de bytes estándar
    dir_servidor.sin_addr.s_addr = inet_addr(argv[1]);//se traduce la IP introducida por el susario a binario

    if(dir_servidor.sin_addr.s_addr == INADDR_NONE)
    {
        printf("La IP introducida no es válida.\n");
        exit(1);
    }

    crear_conexion(establecer_canal(), &dir_servidor);
}
// Angiolina Meyer - Y2872983K

#include<sys/socket.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<arpa/inet.h>

#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>

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


void enlazar_y_escuchar(int conector, struct sockaddr_in *dir_servidor)
{
    if(bind(conector, (struct sockaddr *)dir_servidor, sizeof(*dir_servidor)) < 0)
    {
        perror("Error en el bind");
        exit(1);
    }

    if(listen(conector, 5) < 0)
    {
        perror("Error en el listen");
        exit(1);
    }
}


void enviar_archivo(int socket_cliente)
{
    int archivo = open("Google.html", O_RDONLY);
    if(archivo < 0)
    {
        perror("Error al abrir el archivo");
        close(socket_cliente);
        exit(1);
    }

    char buffer[4096];
    ssize_t leidos;

    while((leidos = read(archivo, buffer, sizeof(buffer))) > 0)
    {
        if(write(socket_cliente, buffer, leidos) < 0)
        {
            perror("Error al escribir en el socket");
            close(archivo);
            close(socket_cliente);
            exit(1);
        }
    }

    if(leidos < 0)
    {
        perror("Error al leer el archivo");
        close(archivo);
        close(socket_cliente);
        exit(1);
    }

    close(archivo);
    close(socket_cliente);
}


void atender_clientes(int conector)
{
    while(true)
    {
        int socket_cliente = accept(conector, NULL, NULL);
        if(socket_cliente < 0)
        {
            perror("Error al aceptar la conexión");
            continue;
        }

        pid_t pid = fork();

        if(pid < 0)
        {
            perror("Error en el fork");
            close(socket_cliente);
        }
        //el hijo cierra el conector y envía el archivo al cliente
        else if(pid == 0)
        {
            close(conector);
            printf("Cliente conectado, enviando archivo.\n");
            enviar_archivo(socket_cliente);
            exit(0);
        }
        //sólo el servidor ejecuta esto
        else
        {
            close(socket_cliente);
            printf("Cliente atendido, esperando nuevas conexiones.\n");
        }
    }
}


int main()
{
    struct sockaddr_in dir_servidor = {0};

    dir_servidor.sin_family = AF_INET;//se usará el protocolo de red IPv4
    dir_servidor.sin_port = htons(PUERTO);//se asigna el puerto y se transforma al orden de bytes estándar
    dir_servidor.sin_addr.s_addr = INADDR_ANY; // acepta conexiones desde cualquier IP de la máquina

    int conector = establecer_canal();
    enlazar_y_escuchar(conector, &dir_servidor);
    atender_clientes(conector);

    close(conector);
    return 0;
}
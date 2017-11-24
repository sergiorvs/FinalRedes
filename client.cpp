#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <thread>
using namespace std;

struct sockaddr_in stSockAddr;
int Res;
int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
int n;
char id_usuario[1];


void writeS()
{
    cout<<"Mi id es: "<<id_usuario[0]<<endl;
    while(true)
    {
        string buffer;
        string letra;
        cin>>letra;

        buffer="";
        buffer[0] = id_usuario[0];
        buffer[0] = letra[0];
        n = write(SocketFD, buffer.c_str(),1);

    }

}
void readS()
{
    while(true)
    {
        string buffer;
        char* buff;
        buff=new char[1];
        n = read(SocketFD,buff,1);
        string aux(buff);
        buffer = aux;  /*El servidor hace broadcast*/
        cout<<"Yo cliente "<<id_usuario<<" obtuve el sgnte msj: "<<buffer<<endl;
        if (n < 0) perror("ERROR reading from socket");
    }

}


int main(void)
{
    if (-1 == SocketFD)
    {
        perror("cannot create socket");
        exit(EXIT_FAILURE);
    }

    memset(&stSockAddr, 0, sizeof(struct sockaddr_in));

    stSockAddr.sin_family = AF_INET;
    stSockAddr.sin_port = htons(2102);
    Res = inet_pton(AF_INET, "172.20.10.7", &stSockAddr.sin_addr);

    if (0 > Res)
    {
        perror("error: first parameter is not a valid address family");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }
    else if (0 == Res)
    {
        perror("char string (second parameter does not contain valid ipaddress");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    if (-1 == connect(SocketFD, (const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
    {
        perror("connect failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    read(SocketFD,id_usuario,1); //el cliente almacena en  id_usuario el id enviado por el servidor

    std::thread t1 (readS);
    std::thread t2 (writeS);
    t1.join();
    t2.join();

    shutdown(SocketFD, SHUT_RDWR);
    close(SocketFD);
    return 0;
}
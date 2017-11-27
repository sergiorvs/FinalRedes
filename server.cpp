#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <chrono>
#include <string>

//Cambiando a C++
#include <iostream>
#include <thread>         // std::thread, std::this_thread::sleep_for
#include <vector>
#include <algorithm>

using namespace std;

struct sockaddr_in stSockAddr;
int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); //Se trabaja con protocolo TCP Handshaking

int n;
vector<int> clients_id; /*Vector que recopila id de los clientes conectados*/
string buffer;

void aceptClient(int ConnectFD) {
    char* buff;
    buff = new char[2]; //El cliente le envia 7, pero solo decide leer los 2 primeros

    while(n = read(ConnectFD, buff, 1) > 0) {
        if (n < 0) perror("ERROR reading from socket");
        string aux(buff);
        buffer = aux;
        aux.clear();
        cout<<"El tam del Vector es: "<<clients_id.size()<<endl;
        for (int i=0; i<clients_id.size(); i++)
        {
            cout<<"Enviando a Cliente: "<<clients_id[i]<<" El msg:  "<<buffer<<endl;
            n = write(clients_id[i], buffer.c_str(), buffer.size());
            if (n < 0) perror("ERROR writing to socket");
        }
        buffer.clear();
    }

    if (n <= 0) {
        auto it = std::find(clients_id.begin(), clients_id.end(), ConnectFD);
        if(it != clients_id.end())
            clients_id.erase(it);

        shutdown(ConnectFD, SHUT_RDWR);
        close(ConnectFD);
        pthread_exit(nullptr);
    }
}

int main()
{
    /*
    This function creates a socket and specifies communication style style,
    which should be one of the socket styles listed in Communication Styles. (TCP en este caso)
    The namespace argument specifies the namespace; it must be PF_LOCAL (see Local Namespace) or PF_INET (see Internet Namespace).
    */

    if(-1 == SocketFD)
    {
        perror("can not create socket");
        exit(EXIT_FAILURE);
    }

    memset(&stSockAddr, 0, sizeof(struct sockaddr_in)); //Separa espcio en memoria para la Estructura

    stSockAddr.sin_family = AF_INET;      //Protocolo para la conexión
    stSockAddr.sin_port = htons(2102);      //Puerto para la conexión
    stSockAddr.sin_addr.s_addr = INADDR_ANY;

    if(-1 == bind(SocketFD,(const struct sockaddr *)&stSockAddr, sizeof(struct sockaddr_in)))
    {
        perror("error bind failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    if(-1 == listen(SocketFD, 10))
    {
        perror("error listen failed");
        close(SocketFD);
        exit(EXIT_FAILURE);
    }

    //Hace que el Servidor siempre escuche
    while(true)
    {
        int ConnectFD = accept(SocketFD, nullptr, nullptr);

        if(0 > ConnectFD) {
            perror("error accept failed");
            close(SocketFD);
            exit(EXIT_FAILURE);
        }
        clients_id.push_back(ConnectFD);

        char id[1];
        sprintf(id,"%d",ConnectFD); //De entero a char
        write(ConnectFD,id,1);      //Luego que el cliente se conecta, el servidor halla su id, y se lo envía.

        std::thread (aceptClient, ConnectFD).detach();
    }
}
/* Server code in C++ */
// Compilar: g++ -std=c++11 -pthread -o s.exe server.cpp
//  Ejecutar: ./s.exe
 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <chrono>
#include <string.h>
#include <string>

//Cambiando a C++
#include <iostream>
#include <thread>         // std::thread, std::this_thread::sleep_for
#include <vector>
using namespace std;


struct sockaddr_in stSockAddr;                
int SocketFD = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP); //Se trabaja con protocolo TCP Handshaking
string buffer;
int n;
vector<int> iD; /*Vector que recopila id de los clientes conectados*/



void keepAlive()
{
    for(int i=0;i<iD.size();i++)
    {
      if(0 > iD[i]) {
        perror("Socket Fuera de Servicio");
        close(SocketFD);
        exit(EXIT_FAILURE);
        int a; cin>>a;
      }
      else
      {
        cout <<iD[i]<<"  Keep Alive "<<endl;
      }
    }
}


void aceptClient(int ConnectFD) {
  do {

    //keepAlive();
    char* buff;    
    buff = new char[2]; //El cliente le envia 7, pero solo decide leer los 2 primeros
    n = read(ConnectFD,buff,1);
    if (n < 0) perror("ERROR reading from socket");
    string aux(buff);
    buffer = aux;
    aux.clear();
    cout<<"El tam del Vector es: "<<iD.size()<<endl;
    for(int i=0;i<iD.size();i++)
    {
        cout<<"Enviando a Cliente: "<<iD[i]<<" El msg:  "<<buffer<<endl;          
        n = write(iD[i],buffer.c_str(),buffer.size());
        if (n < 0) perror("ERROR writing to socket");        
    }
    buffer.clear();
    

 } while(buffer != "END");

    shutdown(ConnectFD, SHUT_RDWR);

    close(ConnectFD); //Cierra el Socket ( Socket : puente para que 2 computadoras se comuniquen remota o localmente) CIERRA la comunicación
}

  

int main(void)
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

while(1)
{
  int ConnectFD = accept(SocketFD, NULL, NULL);
  
  if(0 > ConnectFD) {
    perror("error accept failed");
    close(SocketFD);
    exit(EXIT_FAILURE);
  }
  iD.push_back(ConnectFD);

  char id[1];
  sprintf(id,"%d",ConnectFD); //De entero a char
  write(ConnectFD,id,1);      //Luego que el cliente se conecta, el servidor halla su id, y se lo envía.
  
  //std::thread (keepAlive).detach();  
  std::thread (aceptClient, ConnectFD).detach();
}

close(SocketFD);
return 0;
}
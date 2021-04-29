# TCP-Client-Server

**Client.cpp**
Your client program must receive the following six arguments:

port: a server IP port
repetition: the repetition of sending a set of data buffers

nbufs: the number of data buffers

bufsize: the size of each data buffer (in bytes)

serverIp: a server IP name

type: the type of transfer scenario: 1, 2, or 3 (see below)





**Server**
Your server program must receive the following two arguments:

port: a server IP port

repetition: the repetition of sending a set of data buffers


Following Libraries that were used:
   #include <sys/types.h>    // socket, bind
    #include <sys/socket.h>   // socket, bind, listen, inet_ntoa
    #include <netinet/in.h>   // htonl, htons, inet_ntoa
    #include <arpa/inet.h>    // inet_ntoa
    #include    <netdb.h>     // gethostbyname
    #include    <unistd.h>    // read, write, close
    #include   <strings.h>     // bzero
    #include <netinet/tcp.h>  // SO_REUSEADDR
    #include <sys/uio.h>      // writev

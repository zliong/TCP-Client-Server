#include<iostream>
#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
#include <netdb.h>     // gethostbyname
#include <unistd.h>    // read, write, close
#include <strings.h>     // bzero
#include <netinet/tcp.h>  // SO_REUSEADDR
#include <cstring>
#include <sys/uio.h>      // writev
#include <sys/time.h>     //gettimeofday


using namespace std;

const unsigned int MAX_PORT_SIZE = 65535;
const unsigned int MIN_PORT_SIZE = 100;
//const unsigned int BUF_SIZE = 65535;
int main (int argc, char** argv) {
    if(argc != 7)   //Check if user passes in an argument
    {
        cerr << "Must Pass in parameters for the client in the following: [port] [repetition] [nbufs] [bufsize] [serverIp] [type]" << endl;
        return -1;
    }
    int server_port = atoi(argv[1]); //the port that server would be listening in
    if(server_port > MAX_PORT_SIZE || server_port < MIN_PORT_SIZE) //check to make sure the server port does not exceed maxmimum port size or below the minimum port numbers(avoids port:80 and other values for http or others)
    {
       cerr << "Server port must in between 100 and 65535" << endl;
       return -1;
    }
    int repetition = atoi(argv[2]); //store repetition from user's parameter input
    if(repetition < 1)  //Check to see if the repetition is a positive numerical value
    {
        cerr <<"Repetition must be a positive numerical value greater than 0" << endl;
        return -1;
    }
    
    int nbufs = atoi(argv[3]);  //store the nbufs from user's parameter input
    int bufsize = atoi(argv[4]);    //store the bufsize from user's parameter input
    if((nbufs * bufsize) != 1500)   //Check to make sure that multicity of bufsize and nbufs is equal to 1500
    {
        cerr << "[nbufs] [bufsize] must be in multiplication of 1500" << endl;
        return -1;
    }
    char* server_ip = argv[5];  //store the ip address from user's parameter input
    int type = atoi(argv[6]);
    if( type < 1 || type > 3)   //ensure user inputs proper numerical value for [type]
    {
        cerr << "[type] must be in numerical value ranging from 1 to 3" << endl;
        return -1;
    }
    struct hostent* host = gethostbyname(server_ip);    //Can be local host ip or another ip
    //struct hostent* host = gethostbyname("csslab3.uwb.edu");  //used for testing
    //int server_port = 24689; //used for testing

    sockaddr_in sendSockAddr;
    bzero((char*) &sendSockAddr, sizeof(sendSockAddr));  // zero out the data structure
    sendSockAddr.sin_family = AF_INET;   // using IP
    sendSockAddr.sin_addr.s_addr = inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list)); // listen on any address this computer has
    sendSockAddr.sin_port = htons(server_port);  // set the port to listen on

    int clientSd = socket(AF_INET, SOCK_STREAM, 0); // creates a new socket for IP using TCP
    
    int connectStatus = connect(clientSd, (sockaddr*)&sendSockAddr, sizeof(sendSockAddr));
    if(connectStatus < 0)   //Check if client has successfully connected to server
    {
        cerr << "Failed to connect to server" << endl;
    }
    // else{ //confirm to see if socket has connected
    //     cout << "Connected!" << endl;
    // }
    char databuf[nbufs][bufsize];       //allocate and initialize the databuf for writing
    struct timeval start, lap, date_sending_time, round_time, end;     //intialize the timers (timers have not started)
    int r_seconds, lapped, r_usecs, s_seconds, s_usecs;                 //store the round trip and data-sending times(both in seconds and usecs)
    gettimeofday(&start, NULL); //start timer to begin both the data-sending timer and round-trip timer
    for(int i = 0; i < repetition; i++)
    {
        if(type == 1)   //invoke multiple writes
        {
            for ( int j = 0; j < nbufs; j++ )
                write( clientSd, databuf[j], bufsize );   //send info databuf to the server at a time
        }
        else if(type == 2)  //create a iovec data structure and call writev to send all data bufs at once
        {
            struct iovec vector[nbufs];
            for ( int j = 0; j < nbufs; j++ ) {
                vector[j].iov_base = databuf[j];
                vector[j].iov_len = bufsize;
            }
            writev( clientSd, vector, nbufs );   //send an array of buffers to the server at once
        }
        else    //a single write call
        {
            write(clientSd, databuf, nbufs * bufsize);  //send info to the server
        }
    }
    gettimeofday(&lap, NULL);   //finish transfer time
    //create an int to store info
    int numReads;
    read(clientSd, &numReads, sizeof(numReads));  //retrieve the number of reads that the server had to do
    //cout << bytesRead << endl; //testing purposes
    gettimeofday(&end, NULL);   //stop timer to show the completetion of the round-trip of sending/recieving info back and forth

	//calculate the data-sending time(s_usecs) and round-trip time(r_usecs)
    s_seconds = lap.tv_sec - start.tv_sec;
    s_usecs = lap.tv_usec - start.tv_usec;
    s_usecs += s_seconds * 1000000;
    r_seconds = end.tv_sec - start.tv_sec;
    r_usecs = end.tv_usec - start.tv_usec;
    r_usecs += r_seconds * 1000000;

    //Display info onto console
    cout << "Test " << type << ": Data sending time = " << s_usecs <<" usecs, Round-Trip time = " << r_usecs << " usecs, #reads = " << numReads << endl ;
    //close socket
    close(clientSd);

}
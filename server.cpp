#include<iostream>
#include <sys/types.h>    // socket, bind
#include <sys/socket.h>   // socket, bind, listen, inet_ntoa
#include <netinet/in.h>   // htonl, htons, inet_ntoa
#include <arpa/inet.h>    // inet_ntoa
#include <netdb.h>     // gethostbyname
#include <unistd.h>    // read, write, close
#include <strings.h>     // bzero
#include <netinet/tcp.h>  // SO_REUSEADDR
#include <sys/uio.h>      // writev
#include <pthread.h>
#include <sys/time.h>     //gettimeofday

using namespace std;

const unsigned int BUF_SIZE = 1500;
const unsigned int MAX_PORT_SIZE = 65535;
const unsigned int MIN_PORT_SIZE = 100;
struct threadstruct{
    int sd;
    int reps;
    char* buf;
};
//Function that would read from databuf, received from client's databuf, and would be called by thread_create
void *func (void* ptr)
{
    threadstruct *my_struct = (struct threadstruct*) ptr;   //create a new struct as a temporary
    char buf[BUF_SIZE]; //create a temp buf
    (my_struct)->buf = buf; 
    int count = 0;  //intialize the number of times the server reads count
    struct timeval start, end;  //intialize timers for start and end
    gettimeofday(&start, NULL); //start timer 
    for(int i = 0; i < (my_struct)->reps; i++)  //loop for repetition
    {
           for ( int nRead = 0; 
            ( nRead += read( (my_struct)->sd, (my_struct)->buf, BUF_SIZE - nRead ) ) < BUF_SIZE; //read from the client socket's databuf
            ++count );
            ++count;    //increment count for reps
    }
    write((my_struct)->sd, &count, sizeof(count));  //send the count back to client, which would be stored "int numReads"
    gettimeofday(&end, NULL);   //end timer to indicate that write(send) and read operations were completed 
    int receiving_time = (end.tv_sec - start.tv_sec) * 1000000 +
                                (end.tv_usec - start.tv_usec);  //calculate the data-receiving time in usecs
    cout << "data-receiving time = " << receiving_time << " usec" << endl;   //display the time to the console

    close((my_struct)->sd); //close the client socket
    free(ptr);  //delete the threadstruct to free memory
    return NULL;    
}
int main (int argc, char** argv) {
     if(argc != 3)   //Check if user passes in an argument
    {
        cerr << "Must Pass in parameters for the client in the following: [port] [repetition] " << endl;
        return -1;
    }
    // Create the socket
    int server_port = atoi(argv[1]);
    if(server_port > MAX_PORT_SIZE || server_port < MIN_PORT_SIZE) //check to make sure the server port does not exceed maxmimum port size or below the minimum port numbers(avoids port:80 and other values for http or others)
    {
       cerr << "Server port must in between 100 and 65535" << endl;
       return -1;
    }
    int repetition = atoi(argv[2]);
    if(repetition < 1)  //Check to see if the repetition is a positive numerical value
    {
        cerr <<"Repetition must be a positive numerical value greater than 0" << endl;
        return -1;
    }
    sockaddr_in acceptSock;
    bzero((char*) &acceptSock, sizeof(acceptSock));  // zero out the data structure
    acceptSock.sin_family = AF_INET;   // using IP
    acceptSock.sin_addr.s_addr = htonl(INADDR_ANY); // listen on any address this computer has
    acceptSock.sin_port = htons(server_port);  // set the port to listen on

    int serverSd = socket(AF_INET, SOCK_STREAM, 0); // creates a new socket for IP using TCP

    const int on = 1;

    setsockopt(serverSd, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(int));  // this lets us reuse the socket without waiting for hte OS to recycle it

    // Bind the socket

    bind(serverSd, (sockaddr*) &acceptSock, sizeof(acceptSock));  // bind the socket using the parameters we set earlier
    
    // Listen on the socket
    int n = 5;
    listen(serverSd, n);  // listen on the socket and allow up to n connections to wait.

    // Accept the connection as a new socket

    sockaddr_in newsock;   // place to store parameters for the new connection
    socklen_t newsockSize = sizeof(newsock);
    while (1) {
	    int newSd = accept(serverSd, (sockaddr *)&newsock, &newsockSize);  // grabs the new connection and assigns it a temporary socket
    // Read data from the socket
        threadstruct *serverSocketstruct = new threadstruct;    //create the struct
        //populate the new struct of the client's socket and server's repetition
        serverSocketstruct->sd = newSd;    
        serverSocketstruct->reps = repetition;
        pthread_t rthread;
        if(pthread_create(&rthread, NULL, func, (void *) serverSocketstruct) < 0){  //creation of the pthread and does its thread operation (void * func)
            cerr << "Error in creating thread" << endl;
            return -1;
        };
        //Merge threads back to avoid wasting resources
        pthread_join(rthread, NULL);    
        
        // Close the socket 
	    close(newSd);
    }
    return 0;

}



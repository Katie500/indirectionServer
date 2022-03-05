/* Katherine Knauss
CPSC 441 Assignment 2
Here's an indirection server that connects with a client and three different
microservers. It works lovely.

References: Prof. Carey's code and my TA's help.*/



#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>

#define PORT 44101
#define MAX_MESSAGE_LENGTH 100
#define MAX_BUFFER_SIZE 40

#define TPORT 1234
#define CPORT 2345
#define VPORT 3456
#define PORTT 4567

//make sure to change this to match your own IP
#define SERVER_IP "136.159.5.25"

void translatorService();
void ccS();
void voting(int socket);

int main (int argc, char *argv[])
{
    //TCP socket addr from Carey
    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    printf("Init addresses.\n");

    //translator UDP addressing from Carey
    struct sockaddr_in tserverAddr;
    struct sockaddr *server1;
    int tsocket, i, len = sizeof(tserverAddr);
    char buf[MAX_BUFFER_SIZE];
    int bytes;
    memset((char *) &tserverAddr, 0, sizeof(tserverAddr));
    tserverAddr.sin_family = AF_INET;
    tserverAddr.sin_port = htons(TPORT);
    //static IP instead of ANY
    //use ANY for microservices
    tserverAddr.sin_addr.s_addr = inet_addr(SERVER_IP); //hardcoded
    server1 = (struct sockaddr *) &tserverAddr;

    if ((tsocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        printf("Could not set up t socket!\n");
        return 1;
    }
    printf("t socket created\n");


    //convert UDP addressing from Carey
    struct sockaddr_in cserverAddr;
    struct sockaddr *server2;
    int csocket, j, leng = sizeof(cserverAddr);
    char buf1[MAX_BUFFER_SIZE];
    int bytes1;
    memset((char *) &cserverAddr, 0, sizeof(cserverAddr));
    cserverAddr.sin_family = AF_INET;
    cserverAddr.sin_port = htons(CPORT);
    //static IP instead of ANY
    //use ANY for microservices
    cserverAddr.sin_addr.s_addr = inet_addr(SERVER_IP); //hardcoded
    server2 = (struct sockaddr *) &cserverAddr;

    if ((csocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        printf("Could not set up c socket!\n");
        return 1;
    }
    printf("C socket created\n");

    //vote UDP addressing
    struct sockaddr_in vserverAddr;
    struct sockaddr *server3;
    int vsocket, k, length = sizeof(vserverAddr);
    char buff2[MAX_BUFFER_SIZE];
    int bytes2;
    memset((char *) &vserverAddr, 0, sizeof(vserverAddr));
    vserverAddr.sin_family = AF_INET;
    vserverAddr.sin_port = htons(VPORT);
    //static IP instead of ANY
    //use ANY for microservices
    vserverAddr.sin_addr.s_addr = inet_addr(SERVER_IP); //hardcoded
    server3 = (struct sockaddr *) &vserverAddr;

    if ((vsocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        printf("Could not set up v socket!\n");
        return 1;
    }
    printf("V socket created\n");
    //Turns out I didn't need the set sockopt....
    //set sockopt!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //TCP sockets
    int mainSocket, clientSocket;
    char response[MAX_MESSAGE_LENGTH];

    bzero(response, MAX_MESSAGE_LENGTH);

    //creating a new socket
    mainSocket = socket(AF_INET, SOCK_STREAM, 0);
    //checking that socket was created correctly
    if(mainSocket == -1){
        printf("Parent socket failure\n");
        exit(1);
    }
    printf("Created main socket.\n");

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    //checking that socket was created correctly
    if(clientSocket == -1){
        printf("Child socket failure\n");
    }
    printf("Created client socket.\n");

    //binding the sockets
    if((bind( mainSocket, (struct sockaddr *) &address, sizeof(struct sockaddr_in))) == -1)
    {
        printf("Bind error\n");
        exit(1);
    }
    printf("Binding was successful.\n");

    //listening on chosen ports
    if((listen( mainSocket, 5)) == -1)
    {
        printf("Listening error.\n");
        exit(1);
    }
    printf("Listening...\n");

    //continuous loop
    while(1) 
    {
        //accepting connection
        if((clientSocket = accept(mainSocket, NULL, NULL)) == -1) 
        {
            printf("Error: client socket was not accepted.\n");
            exit(1);
        }
        printf("Accepting incoming connection.\n");

        //receiving request from client
        while(recv(clientSocket, response, MAX_MESSAGE_LENGTH, 0) > 0)
        {
            printf("Received response: %s\n", response);


            //vote
            if(strcmp(response, "v") == 0)
            {
                printf("Voting requested.\n");
                char messageout[MAX_BUFFER_SIZE];
                bzero(messageout, MAX_BUFFER_SIZE);

                char messagein[MAX_BUFFER_SIZE];
                bzero(messagein, MAX_BUFFER_SIZE);

                char DBnum1[MAX_BUFFER_SIZE];
                char HOnum1[MAX_BUFFER_SIZE];
                char JCnum1[MAX_BUFFER_SIZE];
                char BTnum1[MAX_BUFFER_SIZE];

                //sending request to voting server
                printf("Sending request to voting server.\n");
                strcpy(messageout, "request");
                sendto(vsocket, messageout, strlen(messageout), 0, server3, length);
                printf("Request sent.\n");
                
                //receiving encryption key from microserver
                printf("Trying to receive key from microserver.\n");
                recvfrom(vsocket, messagein, MAX_BUFFER_SIZE, 0, server3, &length);
                printf("Key %s received\n", messagein);

                //sending encryption key to client
                printf("Sending encryption key to client.\n");
                send(clientSocket, messagein, strlen(messagein), 0);
                printf("Key sent to client.\n");

                bzero(messagein, MAX_BUFFER_SIZE);

                //receiving encryped vote
                printf("Waiting for vote key.\n");
                recv(clientSocket, messagein, MAX_BUFFER_SIZE, 0);
                printf("Vote key received.\n");

                //sending encrypted vote key to microserver
                printf("Sending vote key to microserver.\n");
                sendto(vsocket, messagein, strlen(messagein), 0, server3, length);

                //receiving totals from microserver                
                recvfrom(vsocket, DBnum1, MAX_BUFFER_SIZE, 0, server3, &length);
                recvfrom(vsocket, HOnum1, MAX_BUFFER_SIZE, 0, server3, &length);
                recvfrom(vsocket, JCnum1, MAX_BUFFER_SIZE, 0, server3, &length);
                recvfrom(vsocket, BTnum1, MAX_BUFFER_SIZE, 0, server3, &length);

                //sending results to the client
                printf("Sending results to client.\n");
                send(clientSocket, DBnum1, strlen(messagein), 0);
                send(clientSocket, HOnum1, strlen(messagein), 0);
                send(clientSocket, JCnum1, strlen(messagein), 0);
                send(clientSocket, BTnum1, strlen(messagein), 0);
                printf("Voting Results:\nDonald Biden: %s\nHillary Obama %s\nJoe Clinton %s\nBarack Trump %s\n", DBnum1, HOnum1, JCnum1, BTnum1);


            }
            //convert
            else if(strcmp(response, "c") == 0)
            {
                printf("Convert requested\n");

                //declaring strings
                char messageout[MAX_BUFFER_SIZE];
                char messagein[MAX_BUFFER_SIZE];
                char amount[MAX_BUFFER_SIZE];
                char source[MAX_BUFFER_SIZE];
                char dest[MAX_BUFFER_SIZE];
                
                //clearing strings (just to be safe)
                bzero(messageout, MAX_BUFFER_SIZE);
                bzero(messagein, MAX_BUFFER_SIZE);
                bzero(amount, MAX_BUFFER_SIZE);
                bzero(source, MAX_BUFFER_SIZE);
                bzero(dest, MAX_BUFFER_SIZE);

                printf("Trying to receive amount.\n");

                //receiving info from the client
                recv(clientSocket, amount, MAX_MESSAGE_LENGTH, 0);
                recv(clientSocket, source, MAX_MESSAGE_LENGTH, 0);
                recv(clientSocket, dest, MAX_MESSAGE_LENGTH, 0);
        
                
                printf("Received amount %s. Sending to microservice.\n", amount);

                //sending amount word to the microservice
                sendto(csocket, amount, strlen(amount), 0, server2, leng);
                printf("Sent to microservice.\n");
                
                printf("Received source %s. Sending to microservice.\n", source);

                //sending amount source to the microservice
                sendto(csocket, source, strlen(source), 0, server2, leng);
                printf("Sent to microservice.\n");

                printf("Received dest %s. Sending to microservice.\n", dest);

                //sending dest word to the microservice
                sendto(csocket, dest, strlen(dest), 0, server2, leng);
                printf("Sent to microservice.\n");

                //clearing just to be safe
                bzero(messagein, MAX_BUFFER_SIZE);

                //receiving conversion
                recvfrom(csocket, messagein, 10, 0, server2, &leng);

                //sending conversion to client
                send(clientSocket, messagein, strlen(messagein), 0);
            

            }
            else if(strcmp(response, "t") == 0)
            {   
                printf("Translator part.\n");
                //declaring messages
                char messageout[MAX_BUFFER_SIZE];
                char messagein[MAX_BUFFER_SIZE];
                
                //clearing messages (just to be safe)
                bzero(messageout, MAX_BUFFER_SIZE);
                bzero(messagein, MAX_BUFFER_SIZE);
                
                //receiving english word from the client
                recv(clientSocket, messagein, MAX_MESSAGE_LENGTH, 0);
                printf("Received english word %s. Sending to microservice.\n", messagein);

                //sending english word to the microservice
                sendto(tsocket, messagein, strlen(messagein), 0, server1, len);

                //clearing the message
                bzero(messagein, MAX_BUFFER_SIZE);

                //receiving the Spanish word from the micro service
                recvfrom(tsocket, messagein, MAX_BUFFER_SIZE, 0, server1, &len);
                printf("Received spanish word %s. Sending to client.\n", messagein);

                //sending the Spanish word to the client
                send(clientSocket, messagein, strlen(messagein), 0);
                

            }
            else{
                printf("No clue what that was suppose to be.\n");
                
            }


            printf("Listening...\n");
        }

    }
    //closing all sockets
    close(clientSocket);
    close(tsocket);
    close(csocket);
    close(mainSocket);
    close(vsocket);


}


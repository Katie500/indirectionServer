/*
Katherine Knauss
Prof: Carey Williamson
CPSC 441

References: Carey's code and help from the TA
*/


#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <time.h>

#define PORT 1234
#define MAX_BUFFER_SIZE 40



int main() 
{
    //copied from wordlen server
    struct sockaddr_in si_server, si_client;
    struct sockaddr *server, *client;
    int s, len = sizeof(si_server);
    char messagein[MAX_BUFFER_SIZE];
    char messageout[MAX_BUFFER_SIZE];
    int bytes;

    //creating socket
    if ((s = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        printf("Could not setup a socket!\n");
        return 1;
    }
    
    //setting the memory
    memset((char *) &si_server, 0, sizeof(si_server));
    si_server.sin_family = AF_INET;
    si_server.sin_port = htons(PORT);
    si_server.sin_addr.s_addr = htonl(INADDR_ANY);
    server = (struct sockaddr *) &si_server;
    client = (struct sockaddr *) &si_client;

    //binding socket s 
    if (bind(s, server, sizeof(si_server)) == -1)
    {
        printf("Could not bind to port %d!\n", PORT);
        return 1;
    }
    //end copy

    //defining strings
    char engWord[MAX_BUFFER_SIZE];
    char spanWord[MAX_BUFFER_SIZE];

    //looping continuously
    while(1)
    {
        //clearing strings just in case
        bzero(spanWord, MAX_BUFFER_SIZE);
        bzero(engWord, MAX_BUFFER_SIZE);

        //receiving the english word from the server
        if ((bytes = recvfrom(s, engWord, MAX_BUFFER_SIZE, 0, client, &len)) < 0)
            {
                printf("Read error!\n");
                return -1;
            }
        printf("English word: %s\n", engWord);

        //we receive the english word and compare it here
        if(strcmp(engWord, "Hello") == 0)
        {
            strcpy(spanWord, "Hola");
        }
        else if(strcmp(engWord, "Bye") == 0)
        {
            strcpy(spanWord, "Adios");
        }
        else if(strcmp(engWord, "Chicken") == 0)
        {
            strcpy(spanWord, "Pollo");
        }
        else if(strcmp(engWord, "Unfortunately") == 0)
        {
            strcpy(spanWord, "Desafortunadamente");
        }
        else if(strcmp(engWord, "Food") == 0)
        {
            strcpy(spanWord, "Comida");
        }
        else 
        {
            //error
            printf("Error: word unknown.\n");
            sprintf(spanWord,"Error word unknown.");
        }
       
        printf("Sending %s to server.\n", spanWord);
        //sending the spanish translation to the server
        sendto(s, spanWord, strlen(spanWord), 0, client, len);
    }
    //closing the server
    close(s);
}

/*
Katherine Knauss
UCID: 30087243
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

#define PORT 2345
#define MAX_BUFFER_SIZE 100


int main () 
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

    //init vars
    char amount[10];
    int amounts;
    char source[10];
    char dest[10];
    float total;

    //looping through continuously
    while(1)
    {
        //clearing arrays just in case
        bzero(amount, 10);
        bzero(source, 10);
        bzero(dest, 10);
        bzero(messageout, MAX_BUFFER_SIZE);

        //receiving amount from server
        recvfrom(s, amount, MAX_BUFFER_SIZE, 0, client, &len);
        printf("Received amount: %s\n", amount);

        //receiving source from server
        recvfrom(s, source, MAX_BUFFER_SIZE, 0, client, &len);
        printf("Received source: %s\n", source);

        //receiving destination from server
        recvfrom(s, dest, MAX_BUFFER_SIZE, 0, client, &len);
        printf("Received destination: %s\n", dest);

        //converting string to int
        amounts = atoi(amount);

        //making sure source is canadian
        if(strcmp(source, "CAD") == 0)
        {
            //US dollar conversion
            if(strcmp(dest, "USD") == 0)
            {
                total = amounts * .81;
                sprintf(messageout, "%f", total);
            }
            //EURO conversion
            else if(strcmp(dest, "EURO") == 0)
            {
                total = amounts * .7;
                sprintf(messageout, "%f", total);
            }
            //POUND conversion
            else if(strcmp(dest, "POUND") == 0)
            {
                total = amounts * .59;
                sprintf(messageout, "%f", total);
            }
            //BITCOIN conversion
            else if(strcmp(dest, "BITCOIN") == 0)
            {
                total = amounts * .000016;
                sprintf(messageout, "%f", total);
            }
            else{
                //sending error to client
                sprintf(messageout, "Currency destination not recognized. Try USD, Euro, Pound, or Bitcoin.\n");

            }

        }
        else
        {
            //sending error to client
            sprintf(messageout, "Source not CAD\n");
        }

        //send this to the client        
        sendto(s, messageout, strlen(messageout), 0, client, len);
    }
    //closing socket
    close(s);
}
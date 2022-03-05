/*
Katherine Knauss
CPSC 441
Prof: Carey Williamson

References: Prof. Carey's code and help from TA's help
*/



#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <time.h>


#define PORT 44101
#define MAX_MESSAGE_LENGTH 100


int main (int argc, char *argv[])
{
    struct sockaddr_in address;
    struct hostent *hp;

    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(PORT);
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    printf("Init addresses.\n");

    int clientSocket;

    char response[MAX_MESSAGE_LENGTH];
    char otherResponse[MAX_MESSAGE_LENGTH];
    int length, bytes;

    //creating socket for the client
    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    //checking that socket was created correctly
    if(clientSocket == -1){
        printf("Child socket failure\n");
        exit(1);
    }
    printf("Created client socket.\n");

    //connecting
    int status = connect(clientSocket, (struct sockaddr *) &address, sizeof(address));
    if( status < 0)
    {
        printf("Client could not connect.\n");
        exit(1);
    }
    printf("Client connection successful.\n");

    while(1)
    {
        printf("Would you like to translate (t), convert (c), or vote (v)? ");
        scanf("%s", &response);
        printf("Received request %s.\n", response);

        //sending what the client wants from the indirection server
        send(clientSocket, response, 1, 0); 

        //voting
        if(strcmp(response, "v") == 0)
            {   
                printf("Received request to vote.\n");
                char encryptionKeys[MAX_MESSAGE_LENGTH];
                int encryptionKey;
                int newKey;
                int vote;

                //init strings
                char DBnum1[MAX_MESSAGE_LENGTH];
                char HOnum1[MAX_MESSAGE_LENGTH];
                char JCnum1[MAX_MESSAGE_LENGTH];
                char BTnum1[MAX_MESSAGE_LENGTH];

                bzero(encryptionKeys, MAX_MESSAGE_LENGTH);

                printf("You chose to vote!\n");
                printf("Your candidates are:\nDonald Biden Voting ID: 1\nHillary Obama Voting ID: 2\nJoe Clinton Voting ID: 3\nBarack Trump Voting ID: 4\n");
                printf("Please wait for encryption key.\n");

                //receiving encryption key
                recv(clientSocket, encryptionKeys, MAX_MESSAGE_LENGTH, 0);
                printf("Voting secured. Please input the voting ID you wish to choose:");
                scanf("%d", &vote);
                printf("\n");

                printf("You Chose voting ID %d\n", vote);

                //change encryptionKeys to int
                encryptionKey = atoi(encryptionKeys);

                //encrupting vote
                newKey = encryptionKey * vote;

                //clearing strings just in case
                bzero(encryptionKeys, MAX_MESSAGE_LENGTH);
                bzero(DBnum1, MAX_MESSAGE_LENGTH);
                bzero(HOnum1, MAX_MESSAGE_LENGTH);
                bzero(BTnum1, MAX_MESSAGE_LENGTH);
                bzero(JCnum1, MAX_MESSAGE_LENGTH);

                //change newKey to string
                sprintf(encryptionKeys, "%d", newKey);
                //sending the vote to the indirection server
                send(clientSocket, encryptionKeys, strlen(encryptionKeys), 0);

                //STILL NEEDS TO RECEIVE THE RESULTS
                recv(clientSocket, DBnum1, MAX_MESSAGE_LENGTH, 0);
                recv(clientSocket, HOnum1, MAX_MESSAGE_LENGTH, 0);
                recv(clientSocket, JCnum1, MAX_MESSAGE_LENGTH, 0);
                recv(clientSocket, BTnum1, MAX_MESSAGE_LENGTH, 0);

                printf("Voting Results:\nDonald Biden: %s\nHillary Obama %s\nJoe Clinton %s\nBarack Trump %s\n", DBnum1, HOnum1, JCnum1, BTnum1);
            }

            //converting
            else if(strcmp(response, "c") == 0)
            {
                printf("Received request to convert.\n");
                char amount[10];
                char source[10];
                char dest[10];
                float total;
                char messagein[MAX_MESSAGE_LENGTH];
                char messageout[MAX_MESSAGE_LENGTH];

                //clearing
                bzero(source, 10);
                bzero(dest, 10);
                bzero(messagein, MAX_MESSAGE_LENGTH);
                bzero(messageout, MAX_MESSAGE_LENGTH);

                //scanning input
                printf("You have chosen the convert service. Please input an amount, the source currency, and the destination currency: (e.g. 10 CAD USD)\n");
                scanf("%s %s %s", &amount, &source, &dest);
                printf("%s %s %s\n", amount, source, dest);

                //sending info to server
                send(clientSocket, amount, strlen(amount), 0);
                printf("Sent amount %s\n", messageout);
                send(clientSocket, source, strlen(source), 0);
                printf("Sent source %s\n", source);
                send(clientSocket, dest, strlen(dest), 0);
                printf("Sent destination %s\n", dest);

                //receiving total
                printf("Trying to receive the total.\n");
                recv(clientSocket, messagein, MAX_MESSAGE_LENGTH, 0);

                //printing results
                printf("Conversion from %s to %s: %s\n", source, dest, messagein);
                


            }
            //translator
            else if(strcmp(response, "t") == 0)
            {
                printf("Received request to translate.\n");
                char engWord[MAX_MESSAGE_LENGTH];
                char spanWord[MAX_MESSAGE_LENGTH];
                printf("Established vars.\n");
                bzero(engWord, MAX_MESSAGE_LENGTH);
                bzero(spanWord, MAX_MESSAGE_LENGTH);
                printf("You have chosen the translator service. \nPlease input an English word: ");
                scanf("%s", &engWord);

                //sending english word to server
                send(clientSocket, engWord, strlen(engWord), 0);

                //receiving Spanish word
                recv(clientSocket, spanWord, MAX_MESSAGE_LENGTH, 0);
                printf("Spanish translation: %s\n", spanWord);

            }

            else
            {
                printf("Else statement reached.\n");
                break;
            }

    }



}

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

#define PORT 3456
#define MAX_BUFFER_SIZE 40
#define MAX_MESSAGE_SIZE 100

int main()
{
    //copied from wordlen server
    struct sockaddr_in si_server, si_client;
    struct sockaddr *server, *client;
    int s, len = sizeof(si_server);
    char messagein[MAX_BUFFER_SIZE];
    char messageout[MAX_BUFFER_SIZE];
    char results[MAX_MESSAGE_SIZE];
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


    //Candidate: Donald Biden Voting ID: 1
    //Candidate: Hillary Obama Voting ID: 2
    //Candidate: Joe Clinton Voting ID: 3
    //Candidate: Barack Trump Voting ID: 4

    //continuous loop
    while(1)
    {
        //clearing strings just in case
        bzero(messageout, MAX_BUFFER_SIZE);
        bzero(messagein, MAX_BUFFER_SIZE);
        bzero(results, MAX_MESSAGE_SIZE);
        
        printf("Listening...\n");
        //receiving the request for an encryption key
        recvfrom(s, messagein, MAX_BUFFER_SIZE, 0, client, &len);

        //initial votes set to zero
        int DBnum = 0, HOnum = 0, JCnum = 0, BTnum = 0;

        if(strcmp(messagein, "request") == 0)
        {

            //encryption key creation
            printf("Creating encryption key.\n");
            int encryptKey = rand() % 10;
            int vote;
            int newKey;

            char encryptKKey [MAX_BUFFER_SIZE];
            char DBnum1[MAX_BUFFER_SIZE];
            char HOnum1[MAX_BUFFER_SIZE];
            char JCnum1[MAX_BUFFER_SIZE];
            char BTnum1[MAX_BUFFER_SIZE];

            printf("encryptedKey: %d\n", encryptKey);


            sprintf(encryptKKey, "%d", encryptKey);

            //send key to client
            printf("Trying to send key to server.\n");
            sendto(s, encryptKKey, strlen(encryptKKey), 0, client, len);
            printf("Sent key to server.\n");

            //client multiplies key with chosen candidate
            //client sends new key back
            printf("Trying to receive response from server.\n");
            recvfrom(s, messageout, MAX_BUFFER_SIZE, 0, client, &len);
            printf("Receiving response from server.\n");

            //convert response to an int
            newKey = atoi(messageout);
            
            //determinint the vote
            vote = newKey/encryptKey;
            printf("Vote: %d\nnewKey:%d\nencryptKey:%d\n", vote, newKey, encryptKey);

            //comparing the vote with candidates
            if(vote == 1)
            {
                DBnum++;
            }
            else if(vote == 2)
            {
                HOnum++;
            }
            else if(vote ==3)
            {
                JCnum++;
            }
            else if(vote ==4)
            {
                BTnum++;
            }
            else{
                printf("Voting error. System compromised.\n");
            }
            //displaying results
            printf("Voting Results:\nDonald Biden: %d\nHillary Obama %d\nJoe Clinton %d\nBarack Trump %d\n", DBnum, HOnum, JCnum, BTnum);
            
            //converting int to string
            sprintf(DBnum1, "%d", DBnum);
            sprintf(HOnum1, "%d", HOnum);
            sprintf(JCnum1, "%d", JCnum);
            sprintf(BTnum1, "%d", BTnum);

            //sending to client
            sendto(s, DBnum1, strlen(DBnum1), 0, client, len);
            sendto(s, HOnum1, strlen(HOnum1), 0, client, len);
            sendto(s, JCnum1, strlen(JCnum1), 0, client, len);
            sendto(s, BTnum1, strlen(BTnum1), 0, client, len);
        }
    }
}
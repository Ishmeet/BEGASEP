#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
#include "begasep_common.h"


/* 
 * error - wrapper for perror
 */
void error(char *msg) {
    perror(msg);
    exit(0);
}

int main(int argc, char **argv) {
    int sockfd, portno, n;
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char *hostname;
    unsigned int betNumber=0;
    /* check command line arguments */
    if (argc != 3) {
        fprintf(stderr,"usage: %s <hostname> <port>\n", argv[0]);
        exit(0);
    }
    hostname = argv[1];
    portno = atoi(argv[2]);

    /* socket: create the socket */
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");

    /* gethostbyname: get the server's DNS entry */
    server = gethostbyname(hostname);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host as %s\n", hostname);
        exit(0);
    }

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
            (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(portno);

    /* connect: create a connection with the server */
    if (connect(sockfd, &serveraddr, sizeof(serveraddr)) < 0) 
        error("ERROR connecting");

    /* get message line from the user */
    Begasep_CommonHeader SendHeader;
    Begasep_CommonHeader RecvHeader;
    makeHeader(PROTOCOL_VERSION, BEGASEP_OPEN, sizeof(SendHeader), 0, &SendHeader);
    if (send(sockfd, (Begasep_CommonHeader*) &SendHeader, sizeof(SendHeader), 0) == -1)
        perror("send");

    printf("Client Sends    **** |Version = %2u | Packet Type = %2u | Packet Length = %d | ClientID = %d | **** \n", SendHeader.Version, SendHeader.Type,SendHeader.Length, SendHeader.ClientId);
    int ResultReceived=0;
    for (;;) {
        n = read(sockfd, &RecvHeader, sizeof(RecvHeader));
        if ( n <= 0) {
            perror("recv invalid Bet\n\r");
            printf("Closing Socket..: %d", sockfd);
            close(sockfd);
            exit(1);
        } 
        else 
        {
            printf("\n\nClient Receives **** |Version = %2u | Packet Type = %2u | Packet Length = %d | ClientID = %d | **** \n",RecvHeader.Version, RecvHeader.Type,RecvHeader.Length, RecvHeader.ClientId);
            switch (RecvHeader.Type) {
                case BEGASEP_ACCEPT: {
                    printf("");
                    Begasep_AcceptMsg AcceptMessage;
                    if ((read(sockfd, &AcceptMessage, sizeof(AcceptMessage))) <= 0) {
                        perror("recv");
                        exit(1);
                    }
                    printf("**** |Minimum Limit = %ld | Maximum Limit  %ld | \n",AcceptMessage.LowerEndofNumber,AcceptMessage.UpperEndofNumber);
                    Begasep_BetMsg BetMessage;
                    betNumber = GenerateBetNumber(BEGASEP_NUM_MIN, BEGASEP_NUM_MAX);
                    printf("Bet number = %ld\n\r", betNumber);
                    BetMessage.ClientBet = betNumber;
                    makeHeader(PROTOCOL_VERSION, BEGASEP_BET,sizeof(SendHeader) + sizeof(BetMessage),RecvHeader.ClientId, &SendHeader);
                    if (send(sockfd, (Begasep_CommonHeader*) &SendHeader,sizeof(SendHeader), 0) == -1)
                        perror("send");
                    if (send(sockfd, (Begasep_BetMsg*) &BetMessage, sizeof(BetMessage),0) == -1)
                        perror("send");
                    printf("\n\nClient Sends    **** |Version = %2u | Packet Type = %2u | Packet Length = %d | ClientID = %d | **** \n", SendHeader.Version, SendHeader.Type,SendHeader.Length, SendHeader.ClientId);
                    printf("**** |Bet made = %ld |\n", BetMessage.ClientBet);
                    break;
                                     }
                case BEGASEP_RESULT: {
                    Begasep_ResultMsg ResultMessage;
                    if ((recv(sockfd, &ResultMessage, sizeof(ResultMessage), 0)) <= 0) {
                        perror("recv");
                        exit(1);
                    }
                    printf("**** |Bet Status %d | Winning number %ld | \n",ResultMessage.ResultStatus, ResultMessage.WinningNumber);
                    if (ResultMessage.ResultStatus == 1)
                        printf("***wow you won the bet***\n");
                    else
                        printf("***:( you lost the bet***\n");

                    ResultReceived = 1;
                    break;
                                     }
            }
            if (ResultReceived == 1)
                break;
        }
    }
    close(sockfd);
    return 0;
}

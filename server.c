#include <stdio.h> 
#include <string.h>   //strlen 
#include <stdlib.h> 
#include <errno.h> 
#include <unistd.h>   //close 
#include <arpa/inet.h>    //close 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <sys/time.h>
#include <netinet/in.h> 
#include <sys/time.h> //FD_SET, FD_ISSET, FD_ZERO macros 
#include <poll.h>
#include "begasep_common.h"

#define TRUE   1 
#define FALSE  0 
#define PORT 2222

int main(int argc , char *argv[])  
{  
    int opt = TRUE;  
    int master_socket , addrlen , new_socket , client_socket[40] , 
    max_clients = 40 , activity, i , valread , sd;  
    int max_sd;  
    struct sockaddr_in address;  
    struct timeval timeout;
    unsigned int winningNumber=0;
    unsigned int win_num[40];

    //set of socket descriptors 
    fd_set readfds;  

    //initialise all client_socket[] to 0 so not checked 
    for (i = 0; i < max_clients; i++)  
    {  
        client_socket[i] = 0;  
    }  

    //create a master socket 
    if( (master_socket = socket(AF_INET , SOCK_STREAM , 0)) == 0)  
    {  
        perror("socket failed");  
        exit(EXIT_FAILURE);  
    }  

    //set master socket to allow multiple connections , 
    //this is just a good habit, it will work without this 
    if( setsockopt(master_socket, SOL_SOCKET, SO_REUSEADDR, (char *)&opt, 
                sizeof(opt)) < 0 )  
    {  
        perror("setsockopt");  
        exit(EXIT_FAILURE);  
    }

    //type of socket created 
    address.sin_family = AF_INET;  
    address.sin_addr.s_addr = INADDR_ANY;  
    address.sin_port = htons( PORT );  

    //bind the socket to localhost port 8888 
    if (bind(master_socket, (struct sockaddr *)&address, sizeof(address))<0)  
    {  
        perror("bind failed");  
        exit(EXIT_FAILURE);  
    }  
    printf("Listener on port %d \n", PORT);  

    //try to specify maximum of 3 pending connections for the master socket 
    if (listen(master_socket, 3) < 0)  
    {  
        perror("listen");  
        exit(EXIT_FAILURE);  
    }  

    //accept the incoming connection 
    addrlen = sizeof(address);  
    puts("Waiting for connections ...");  

    winningNumber = generateWinningNumber(BEGASEP_NUM_MIN, BEGASEP_NUM_MAX);

    //clear the socket set 
    FD_ZERO(&readfds);  
    //add master socket to set 
    FD_SET(master_socket, &readfds);  
    max_sd = master_socket;
   
    while(TRUE)  
    { 
        //clear the socket set 
        FD_ZERO(&readfds);  
        //add master socket to set 
        FD_SET(master_socket, &readfds);  
        max_sd = master_socket;
        
        //add child sockets to set 
        for ( i = 0 ; i < max_clients ; i++)  
        {  
            //socket descriptor 
            sd = client_socket[i];  

            //if valid socket descriptor then add to read list 
            if(sd > 0)  
                FD_SET( sd , &readfds);  

            //highest file descriptor number, need it for the select function 
            if(sd > max_sd)  
                max_sd = sd;  
        }  

        timeout.tv_sec = 15;
        timeout.tv_usec = 0;
        //wait for an activity on one of the sockets , timeout is 15 secs , 
        activity = select( max_sd + 1 , &readfds , NULL , NULL , &timeout);  

        if ((activity < 0) && (errno!=EINTR))  
        {  
            printf("select error");  
        }  
        else if ( activity == 0 ) 
        {
            for (i = 0; i < max_clients; i++)  
            {  
                if ((sd = client_socket[i]) && (i != master_socket))
                {    
                    if(win_num[sd] == -1) {
                        close(sd);
                        FD_CLR(sd, &readfds);
                    }
                    printf("Sending Result message...: %ld\n\r", winningNumber);
                    Begasep_ResultMsg ResultMessage;
                    ResultMessage.WinningNumber = winningNumber;
                    if (winningNumber == win_num[sd])
                        ResultMessage.ResultStatus = 1; //if the submitted bet is equal to the winning number
                    else
                        ResultMessage.ResultStatus = 0;
                    //the final BEGASEP_RESULT message is sent to all connected client who did submit a reques
                    Begasep_CommonHeader SendHeader;
                    makeHeader(PROTOCOL_VERSION, BEGASEP_RESULT, sizeof(SendHeader) + sizeof(ResultMessage), sd, &SendHeader);
                    if (send(sd, (Begasep_CommonHeader*) &SendHeader, sizeof(SendHeader), 0) == -1) 
                        perror("Send error Common header of Result message\n\r");
                        if (send(sd, (Begasep_ResultMsg*) &ResultMessage, sizeof(ResultMessage), 0) == -1) 
                            perror("Send error Result Message\n\r");

                            printf("\n\nServer Sends time out msg   **** |Version = %2u | Packet Type = %2u | Packet Length = %d | ClientID = %d | **** \n", SendHeader.Version, SendHeader.Type,SendHeader.Length, sd);
                            printf("**** |Bet Status %d | Winning number %ld | \n",ResultMessage.ResultStatus,ResultMessage.WinningNumber);

                }
            }
            winningNumber = generateWinningNumber(BEGASEP_NUM_MIN, BEGASEP_NUM_MAX);
        }

        //If something happened on the master socket , 
        //then its an incoming connection 
        if (FD_ISSET(master_socket, &readfds))  
        {  
            if ((new_socket = accept(master_socket, 
                            (struct sockaddr *)&address, (socklen_t*)&addrlen))<0)  
            {  
                perror("accept");  
                exit(EXIT_FAILURE);  
            }  

            //inform user of socket number - used in send and receive commands 
            printf("New connection , socket fd is %d , ip is : %s , port : %d \
                    \n" , new_socket , inet_ntoa(address.sin_addr) , ntohs  \
                    (address.sin_port));  

            puts("Welcome message sent successfully");  

            //add new socket to array of sockets 
            for (i = 0; i < max_clients; i++)  
            {  
                //if position is empty 
                if( client_socket[i] == 0 )  
                {  
                    client_socket[i] = new_socket;  
                    printf("Adding to list of sockets as %d\n" , i);  

                    break;  
                }  
            }  
        }  

        //else its some IO operation on some other socket
        for (i = 0; i < max_clients; i++)  
        {  
            sd = client_socket[i];  

            if (FD_ISSET( sd , &readfds))  
            {  
                Begasep_CommonHeader RecvHeader;
                Begasep_CommonHeader SendHeader;
                //Check if it was for closing , and also read the 
                //incoming message 
                if ((valread = read( sd , &RecvHeader, sizeof(RecvHeader))) == 0)  
                {  
                    //Somebody disconnected , get his details and print 
                    getpeername(sd , (struct sockaddr*)&address , \
                            (socklen_t*)&addrlen);  
                    printf("Host disconnected , ip %s , port %d \n" , 
                            inet_ntoa(address.sin_addr) , ntohs(address.sin_port));  

                    //Close the socket and mark as 0 in list for reuse 
                    close( sd );  
                    client_socket[i] = 0;  
                }  

                //Echo back the message that came in 
                else
                {  
                    printf("\n\nServer Receives **** |Version = %2u | Length = %2u | Type = %d | ClientID = %d | **** \n",RecvHeader.Version,RecvHeader.Length, RecvHeader.Type,RecvHeader.ClientId);
                    switch(RecvHeader.Type) {
                        case BEGASEP_OPEN: {
                            Begasep_AcceptMsg AcceptMessage;
                            AcceptMessage.LowerEndofNumber = BEGASEP_NUM_MIN;
                            AcceptMessage.UpperEndofNumber = BEGASEP_NUM_MAX;
                            //header for BEGASEP_ACCEPT is created
                            makeHeader(PROTOCOL_VERSION, BEGASEP_ACCEPT,sizeof(SendHeader) + sizeof(AcceptMessage), sd, &SendHeader);
                            //header for the BEGASEP_ACCEPT is sent to the client
                            if (send(sd, (Begasep_CommonHeader*) &SendHeader,sizeof(SendHeader), 0) == -1)
                                perror("send");
                            //in response BEGASEP_ACCEPT is sent to the client
                            if (send(sd, (Begasep_AcceptMsg*) &AcceptMessage,sizeof(AcceptMessage), 0) == -1)
                                perror("send");
                            printf("\n\nServer Sends    **** |Version = %2u | Packet Type = %2u | Packet Length = %d | ClientID = %d | **** \n",SendHeader.Version,SendHeader.Type,SendHeader.Length,SendHeader.ClientId);
                            printf("**** | Minimum Limit = %ld | Maximum Limit  %ld | \n",AcceptMessage.LowerEndofNumber,AcceptMessage.UpperEndofNumber);
                            break;
                                           }
                        case BEGASEP_BET: {
                            Begasep_BetMsg BetMessage;
                            if ((recv(sd, &BetMessage, sizeof(BetMessage), 0))<= 0) {
                                perror("recv");
                                exit(1);
                            }
                            printf("**** |Bet made = %ld |\n",BetMessage.ClientBet);
                            //this is a case if the bet made by the client is not within the specified limit so the socket is simply closed
                            if (BetMessage.ClientBet <= BEGASEP_NUM_MIN || BetMessage.ClientBet >= BEGASEP_NUM_MAX) {
                                printf("Bet made was not within the limit\n");
                                win_num[sd] = -1;
                                close(sd);
                                FD_CLR(sd, &readfds);
                            } else {
                                printf("Client made a valid Bet\n");
                                win_num[sd] = BetMessage.ClientBet;
                            }
                            break;
                                          }
                        default:
                            printf("Invalid Type received!\n\r");
                    }
                }  
            }  
        }  
    }  
    return 0;  
}  

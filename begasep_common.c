#include "begasep_common.h"
#include <stdio.h>
#include <string.h>   //strlen
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>   //close
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <poll.h>


//this function is used to make up the header of the 4 messages specified, as all messages share a common header
void makeHeader(unsigned ProtocolVersion, unsigned PacketType, uint8_t PacketSIze, uint16_t ClientID, Begasep_CommonHeader *SendHeader) {
    SendHeader->Version = ProtocolVersion;
    SendHeader->Type = PacketType;
    SendHeader->Length = PacketSIze;
    SendHeader->ClientId = ClientID;
}

unsigned int generateWinningNumber(unsigned int min, unsigned int max) {
    static int Init = 0;
    if (Init == 0) {
        srand(time(NULL));
        Init = 1;
    }
    return (rand() % (max - min + 1) + min);
}

int GenerateBetNumber(int min, int max) {
    static int Init = 0;
    if (Init == 0) {
        srand(time(NULL));
        Init = 1;
    }
    return (rand() % (max - min + 1) + min);
}


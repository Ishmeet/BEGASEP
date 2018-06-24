#ifndef BEGASEP_LOGIC_H
#define BEGASEP_LOGIC_H

// 0                   1                   2                   3
// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                   Lower end of number range                   |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |                   Upper end of number range                   |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
typedef struct BEGASEP_ACCEPTMSG {
    unsigned int LowerEndofNumber;
    unsigned int UpperEndofNumber;
} Begasep_AcceptMsg;


//0                   1                   2                   3
//0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
//|                       Betting number                          |
//+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
typedef struct BEGASEP_BETMSG {
    unsigned int ClientBet;
} Begasep_BetMsg;

// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// | Ver |  Len    |    Type       |        Client Id              |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
typedef struct BEGASEP_COMMONHEADER {
    unsigned Version :3;  //3bits
    unsigned Length :5;       //4 bits
    unsigned char Type;         //1 byte
    unsigned short ClientId;            //2byte
} Begasep_CommonHeader;

// 0                   1                   2                   3
// 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// |    Status     |                Winning number...              |
// +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
// | Win num cont. |
// +-+-+-+-+-+-+-+-+

typedef struct BEGASEP_RESULTMSG {
    unsigned char ResultStatus;  //1byte
    unsigned int WinningNumber;
} Begasep_ResultMsg;

///////Message Types////////
#define BEGASEP_OPEN    0x1
#define BEGASEP_ACCEPT  0x2
#define BEGASEP_BET     0x3
#define BEGASEP_RESULT  0x4

#define BEGASEP_NUM_MIN      0xe0ffff00
#define BEGASEP_NUM_MAX      0xe0ffffaa
#define PROTOCOL_VERSION 0x1

// PROTOTYPES

void makeHeader(unsigned ProtocolVersion, unsigned PacketType, unsigned char PacketSIze, unsigned short ClientID, Begasep_CommonHeader *SendHeader);

unsigned int generateWinningNumber(unsigned int min, unsigned int max);
unsigned int generateBetNumber(unsigned int min, unsigned int max);

#endif

#include <stdint.h>
#include <netinet/in.h>
#include <iostream>

#include <list>

/* entete du paquet */
typedef struct {
  uint16_t packetLength;
  uint16_t packetSequenceNumber;
}packetHeader;

/* entete generique du packet */
typedef struct{
  uint8_t messageType;
  uint8_t vTime;
  uint16_t messageTime;
  in6_addr originatorAddress;
  uint8_t timeToLive;
  uint8_t hopCount;
  uint16_t messageSequenceNumber;
}messageHeader;

typedef struct{
  uint8_t linkCode;
  uint8_t reserved;
  uint16_t linkMessageSize;
  std::list<in6_addr> neighborsAddrList;
}helloNeighborList;

/* entete de message hello */
typedef struct{
  uint16_t reserved1;
  uint8_t hTime;
  uint8_t willingness;
  std::list<helloNeighborList> neighbors;
}helloMessageHeader;
/* link message */

/* entete de message */

typedef struct{
  uint16_t ANSN;
  uint16_t reserved;
  std::list<in6_addr> advertisedNeighborMainAddress ;
}tcMessageHeader;
#ifndef HEADER_H
#define HEADER_H

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
typedef struct {
  uint8_t messageType;
  uint8_t vTime;
  uint16_t messageSize;
  in6_addr originatorAddress;
  uint8_t timeToLive;
  uint8_t hopCount;
  uint16_t messageSequenceNumber;
} messageHeader;

typedef struct {
  uint8_t linkCode;
  uint8_t reserved;
  uint16_t linkMessageSize;
  in6_addr* neighborsAddrList;// array of neighbors (ipv6)
  int sizeNAL;// size of the array neighborsAddrList
} helloNeighborList;

/* entete de message hello */
typedef struct {
  uint16_t reserved;
  uint8_t hTime;
  uint8_t willingness;
  helloNeighborList* neighbors;// array of the neighbors (1 helloNeighborList by interface)
  int sizeNL; // size of the array neighbors
} helloMessageHeader;
/* link message */

/* entete de message */
typedef struct {
  uint16_t ANSN;
  uint16_t reserved;
  in6_addr* advertisedNeighborMainAddress ;// tableau d'IPV6ADDR
  int sizeANMA;// size of the arrray advertisedNeighborMainAddress
}tcMessageHeader;

typedef union {
  tcMessageHeader* bodyTC;
  helloMessageHeader* bodyHello;
} msgBody;

#endif

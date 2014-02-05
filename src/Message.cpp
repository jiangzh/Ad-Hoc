/* includes généraux */
#include "Message.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <list>
#include <math.h>
#include <cstdlib>

/* includes spécifiques pour obtenir l'ip */
#include <sys/types.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <ifaddrs.h>

/* constantes */
#define SIZE_MAX_UDP 512
#define C_TIME 0.0625
#define a 8
#define b 6


using namespace std;

/* entete de message */
typedef struct {
  uint16_t packetLength;
  uint16_t packetSequenceNumber;
}packetHeader;

/* entete generique de message */
typedef struct{
  uint8_t messageType;
  uint8_t vTime;
  uint16_t size;
  in6_addr originatorAddress;
  uint8_t timeToLive;
  uint8_t hopCount;
  uint16_t messageSequenceNumber;
}messageHeader;

/* entete de hello */
typedef struct{
  uint16_t reserved;
  uint8_t hTime;
  uint8_t willingness;
  std::list<in6_addr> helloNeighborlist;
}helloMessageHeader;

/* list des voisins */
typedef struct{
  uint8_t linkCode;
  uint8_t reserved;
  uint16_t linkMessageSize;
  std::list<in6_addr> neighborList;
}helloNeighborList;

/* entete de TC */
typedef struct{
  uint16_t ANSN;
  uint16_t reserved;
  std::list<in6_addr> neighborList;
}tcMessageHeader;

int envoi_message (void *, char, int);
int envoi_tc (uint16_t, std::list<in6_addr>);
int envoi_hello (uint8_t, uint8_t, std::list<in6_addr>);
in6_addr getMyIp();

/* fonction de construction de TC */
int envoi_tc (uint16_t ANSN, std::list<in6_addr> neighborList)
{
	char type = 2; //TC
	tcMessageHeader tcH;
	tcH.ANSN = ANSN;
	tcH.reserved = 0000000000000000; //toujours cette valeur

	//taille max 512 octets, 1 ip = 16 octets, 31 ip par paquet
	int nbAdd = 0; //compteur d'adresses
	std::list<in6_addr> small_list_ip; //sous liste d'adresses ip

	std::list<in6_addr>::const_iterator
        lit (neighborList.begin()),
        lend(neighborList.end());

	for(;lit!=lend;++lit)
	{
		if (nbAdd == 31) //nombre max d'ip atteintes
		{
			envoi_message(tcH, type, nbAdd*16); //envoi de res
			nbAdd = 0; //reinitialisation de nbAdd
            small_list_ip.clear();
		}

		small_list_ip.push_back(*lit); //adresse -> list
        nbAdd++; //+1 adresse ajoutée
	}

	envoi_message(tcH, type, nbAdd*16); //envoi du restant

	return 0;
}

/* fonction de construction de hello */
int envoi_hello (uint8_t hTime, uint8_t willingness, std::list<in6_addr> helloNeighborlist)
{
	char type = 1; //Hello
	helloMessageHeader helloH;
	helloH.reserved = 0000000000000000; //toujours cette valeur
	helloH.hTime = hTime; //définit par le controller
	helloH.willingness = willingness;

	std::list<in6_addr>::const_iterator
        lit_h (helloNeighborlist.begin()),
        lend_h(helloNeighborlist.end());

	for(;lit_h!=lend_h;++lit_h)
	{
		helloNeighborList helloN;
		helloN.linkCode = *lit_h.linkCode; // inf ou sup à 15 ? -> a resoudre
		helloN.reserved = *lit_h.reserved;
		helloN.linkMessageSize = *lit_h.linkMessageSize;

		std::list<in6_addr>::const_iterator
            lit_ip (*lit_h.neighborList.begin()),
            lend_ip(*lit_h.neighborList.end());

        std::list<in6_addr> list_ip;

		for(;lit_ip!=lend_ip;++lit_ip)
			{
				list_ip.push_back(*lit_ip); //concatenation des adresses
			}

		envoi_message(&helloN, type); //envoi du message Hello
		list_ip.clear();
	}

	return 0;
}

/* fonction de construction de message */
int envoi_message (void * struct_tc_hello, char typen, int sizeAdd)
{
	messageHeader message;

	if (type == 2) //TC
	{
		message.timeToLive = 255; //valeur constante definie dans la RFC
		message.size = 4+sizeAdd;
	}

	else if (type == 1) //Hello
	{
		message.timeToLive = 1; //valeur constante definie dans la RFC
		message.size = 4+
	}

	message.messageType = type;
	message.vTime = C_TIME*(1+a/16)* pow(2,b); //comprendre la magie de a et b
	message.originatorAddress = getMyIp(); //fonction à vérifier
	message.hopCount = 0;
	message.messageSequenceNumber = 0; //numero unique, aucune information pour le traiter
    return 0;
}

/* récupère l'adresse du noeud et la renvoi */
in6_addr getMyIp()
{
	struct ifaddrs *myaddrs, *ifa;
	in6_addr *in_addr;

	if(getifaddrs(&myaddrs) != 0) //pas d'adresse ou impossible d'obtenir la liste
	{
		exit(1);
	}

	for (ifa = myaddrs; ifa != NULL; ifa = ifa->ifa_next) //boucle sur les adresses
	{
		if (ifa->ifa_addr == NULL)
			continue;
		if (!(ifa->ifa_flags & IFF_UP))
			continue;

		switch (ifa->ifa_addr->sa_family)
		{
			case AF_INET: //IPv4
			{
				/*struct sockaddr_in *s4 = (struct sockaddr_in *)ifa->ifa_addr;
				in_addr = &s4->sin_addr;
				break;*/
				exit(-1);
			}

			case AF_INET6: //IPv6
			{
				struct sockaddr_in6 *s6 = (struct sockaddr_in6 *)ifa->ifa_addr;
				in_addr = &s6->sin6_addr;
				break;
			}

			default:
				continue;
		}
	}

	return *in_addr;
}

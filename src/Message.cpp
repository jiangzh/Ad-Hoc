/* includes généraux */
#include "Message.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <list>
#include <math.h>
#include <cstdlib>
#include "headers.h"
#include <string.h>

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

/*variables globales*/
uint16_t packetSequenceNumber = 0;
uint16_t messageSequenceNumber = 0;


using namespace std;

Message::Message(packetHeader* pH, messageHeader* mH, msgBody* mTH)
{
    mPH = pH;
    mMH = mH;
    mMTH = mTH;
}

/* fonction de construction de TC */
int Message::build_tc (uint16_t ANSN, in6_addr* advertisedNeighborMainAddress, int sizeANMA, in6_addr dest_ip)
{
	char type = 2; //TC
	tcMessageHeader tcH;
	tcH.ANSN = ANSN;
	tcH.reserved = 0000000000000000; //toujours cette valeur

	//taille max 512 octets, 1 ip = 16 octets, 31 ip par paquet
	int nbAdd = 0; //compteur d'adresses
	in6_addr* small_list_ip; //sous liste d'adresses ip

	for(int i = 0; i < sizeANMA; i++)
	{
		if (nbAdd == 31) //nombre max d'ip atteintes
		{
			send_message(&tcH, type, nbAdd*16, dest_ip); //envoi de res
			nbAdd = 0; //reinitialisation de nbAdd
            memset (small_list_ip, 0, sizeof(small_list_ip));
		}

		small_list_ip[i] = advertisedNeighborMainAddress[i]; //adresse -> list
        nbAdd++; //+1 adresse ajoutée
	}

	send_message(&tcH, type, nbAdd*16, dest_ip); //envoi du restant

	return 0;
}

/* fonction de construction de hello */
int Message::build_hello (uint8_t hTime, uint8_t willingness, helloNeighborList* neighbors, int sizeNL, in6_addr dest_ip)
{
	char type = 1; //Hello
	helloMessageHeader helloH;
	helloH.reserved = 0000000000000000; //toujours cette valeur
	helloH.hTime = hTime; //définit par le controller
	helloH.willingness = willingness;

	for(int i = 0; i < sizeNL; i++)
	{
		helloNeighborList helloN;
		helloN.linkCode = neighbors[i].linkCode; // inf ou sup à 15 ? -> a resoudre
		helloN.reserved = neighbors[i].reserved;
		helloN.linkMessageSize = neighbors[i].linkMessageSize;

		int nbAdd = 0;

        in6_addr* list_ip;

		for(int j = 0; j < sizeof(neighbors[i].neighborsAddrList); j++)
			{
				list_ip[j] = neighbors[i].neighborsAddrList[j]; //concatenation des adresses
				nbAdd++;
			}

		send_message(&helloN, type, nbAdd*16, dest_ip); //envoi du message Hello
		memset (list_ip, 0, sizeof(list_ip));
	}

	return 0;
}

/* fonction de construction de message */
int Message::send_message (void *htc, char type, int sizeAdd, in6_addr dest_ip)
{
	messageHeader message;
	packetHeader packet;

	if (type == 2) //TC
	{
		message.timeToLive = 255; //valeur constante definie dans la RFC
		message.messageSize = 4+sizeAdd;
	}

	else if (type == 1) //Hello
	{
		message.timeToLive = 1; //valeur constante definie dans la RFC
		message.messageSize = 8+sizeAdd; //a completer
	}

	message.messageType = type;
	message.vTime = C_TIME*(1+a/16)* pow(2,b);
	message.originatorAddress = getMyIp(); //fonction à vérifier
	message.hopCount = 0;
	message.messageSequenceNumber = messageSequenceNumber++; //numero unique, aucune information pour le traiter

    packet.packetLength = sizeof(message) + sizeof(&htc);
    packet.packetSequenceNumber = packetSequenceNumber++;

    /* envoi du paquet sous la forme packetHeader + messageBody + TC/HelloBody + IP de destination */
    sendPacket(packet, message, htc, dest_ip);
    return 0;
}

/* récupère l'adresse du noeud et la renvoi */
in6_addr Message::getMyIp()
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

/* includes généraux */
#include "Message.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <list>
#include <math.h>
#include <cstdlib>
#include <headers.h>
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


using namespace std;

int envoi_message (void *, char, int);
int envoi_tc (uint16_t, in6_addr*, int);
int envoi_hello (uint8_t, uint8_t, helloNeighborList*, int);
in6_addr getMyIp();

/* fonction de construction de TC */
int envoi_tc (uint16_t ANSN, in6_addr* advertisedNeighborMainAddress, int sizeANMA)
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
			envoi_message(&tcH, type, nbAdd*16); //envoi de res
			nbAdd = 0; //reinitialisation de nbAdd
            memset (small_list_ip, 0, sizeof(small_list_ip));
		}

		small_list_ip[i] = advertisedNeighborMainAddress[i]; //adresse -> list
        nbAdd++; //+1 adresse ajoutée
	}

	envoi_message(&tcH, type, nbAdd*16); //envoi du restant

	return 0;
}

/* fonction de construction de hello */
int envoi_hello (uint8_t hTime, uint8_t willingness, helloNeighborList* neighbors, int sizeNL)
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

		envoi_message(&helloN, type, nbAdd*16); //envoi du message Hello
		memset (list_ip, 0, sizeof(list_ip));
	}

	return 0;
}

/* fonction de construction de message */
int envoi_message (void *htc, char type, int sizeAdd)
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
		message.size = 8+sizeAdd; //a completer
	}

	message.messageType = type;
	message.vTime = C_TIME*(1+a/16)* pow(2,b);
	message.originatorAddress = getMyIp(); //fonction à vérifier
	message.hopCount = 0;
	message.messageSequenceNumber = 0; //numero unique, aucune information pour le traiter

	//appel de la fonction qui gère les socket, voir avec Aminatou
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

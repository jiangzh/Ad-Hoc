/* includes généraux */
#include "message.h"
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>

/* includes spécifiques pour obtenir l'ip */
#include <sys/types.h>
#include <arpa/inet.h>
#include <net/if.h>
#include <ifaddrs.h>

/* constantes */
#define SIZE_MAX_UDP 512
#define C_TIME 0.0625


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


/* fonction de construction de TC */
int envoi_tc (uint16_t ANSN, std::list<in6_addr> neighborList)
{
	char type = 2 //TC
	tcMessageHeader tcH;
	tcH.ANSN = ANSN;
	tcH.reserved = 0000000000000000; //toujours cette valeur
	
	//taille max 512 octets, 1 ip = 16 octets, 31 ip par paquet
	int nbAdd = 0; //compteur d'adresses
	std::string res;
	res += tcH.ANSN; //assignation des champs de tcH
	res += tcH.reserved;
	
	for (std::list<int>::iterator it = neighborList.begin(); it != neighborList.end(); it++)
	{	
		if (nbAdd == 31)
		{
			envoi_message(res); //envoi de res
			nbAdd = 0; //reinitialisation de nbAdd
			res = tcH.ANSN; //reinitialisation de res
			res += tcH.reserved;
		}	
		res += *it //adresse -> res
		nbAdd++ //+1 adresse ajoutée
	}
	
	envoi_message(res, type); //envoi du restant
	
	return 0;
}

/* fonction de construction de hello */
int envoi_hello (uint8_t hTime, uint8_t willingness, std::list<in6_addr> helloNeighborlist)
{
	char type = 1 //Hello
	helloMessageHeader helloH;
	helloH.reserved = 0000000000000000; //toujours cette valeur
	helloH.hTime = hTime; //définit par le controller
	helloH.willingness = willingness;
	
	std::string res; //resultat à envoyer
	
	for (std::list<int>::iterator it = helloNeighborList.begin(); it != helloNeighborList.end(); it++) //iteration sur les parties du message
	{
		helloNeighborList helloN;
		helloN.linkCode = *it.linkCode; // inf ou sup à 15 ? -> a resoudre
		helloN.reserved = *it.reserved;
		helloN.linkMessageSize = *it.linkMessageSize;
		
		res += helloN.linkCode;
		res += helloN.reserved;
		res += helloN.linkMessageSize;
  
		for (std::list<int>::iterator itN = neighborList.begin(); itN != neighborList.end(); itN++)
			{
				res += *itN; //concatenation des adresses
			}
			
		envoi_message(res, type); //envoi du message Hello
		res = "";
	}
		
	return 0;
}

/* fonction de construction de message */
int envoi_message (std::string spe, char type)
{
	messageHeader message;
	
	if (type == 2) //TC
	{
		message.timeToLive = 255; //valeur constante definie dans la RFC
	}
	
	else if (type == 1) //Hello
	{
		message.timeToLive = 1; //valeur constante definie dans la RFC
	}
	
	message.size = sizeof(spe);
	message.messageType = type;
	message.vTime = math.pow(C_TIME*(1+a/16)*2,b); //comprendre la magie de a et b
	message.originatorAddress = getMyIp(); //fonction à vérifier
	message.hopCount = 0;
	message.messageSequenceNumber = ; //numero unique, aucune information pour le traiter
	
}

/* récupère l'adresse du noeud et la renvoi */
in6_addr getMyIp()
{
	struct ifaddrs *myaddrs, *ifa;
	void *in_addr;
	char buf[64];

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
				struct sockaddr_in *s4 = (struct sockaddr_in *)ifa->ifa_addr;
				in_addr = &s4->sin_addr;
				break;
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

	return in_addr;
}
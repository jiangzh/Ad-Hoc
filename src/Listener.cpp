#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <boost/asio.hpp>
#include "Message.h"
#include "Listener.h"
#include "headers.h"

#define HELLO_MESSAGE 1
#define TC_MESSAGE 2

namespace listener{
	Listener::~Listener(){
		delete mIoService ;
		delete mRemoteEndpoint;
		delete mSenderEndpoint;
		delete mSocket;
	}
	
	Listener::Listener() {
		try {
			this->mIoService = new  boost::asio::io_service();
			this->mRemoteEndpoint = new boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v6(), 7171);
			this->mSocket = new boost::asio::ip::udp::socket(*this->mIoService,*this->mRemoteEndpoint);
		} catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
		}
	}
  
	void Listener::listenSocket() {
    	boost::system::error_code error;
		try {
			for(;;) {
			  	boost::array <char,BUFF_SIZE> recvBuf;
				size_t len = mSocket->receive_from(boost::asio::buffer(recvBuf), *mRemoteEndpoint,0,error);
			
				if (error && error != boost::asio::error::message_size)
					throw boost::system::system_error(error);
				  
				std::cout<<"taille message : "<<len<<"\n"<<recvBuf.data()<<"\n";
			
				char* temp = recvBuf.data();
				packetHeader* pH;
				messageHeader* mH;
				pH = (packetHeader*)temp;
				temp+=4;
				mH = (messageHeader*)temp;
				temp+=24;
			
				if (mH->messageType == HELLO_MESSAGE) {
					helloMessageHeader* hMH;
					hMH->reserved1 = (uint16_t)*temp;
					temp += 2;
					hMH->hTime = (uint8_t)*temp;
					++ temp;
					hMH->willingness = (uint8_t)*temp;
					++ temp;
					hMH->neighbors = (helloNeighborList*)temp;
				
					uint16_t compt = 4;
				
					while (compt != mH->messageSize) {
						hMH->neighbors->linkCode = (uint8_t)*temp;
						++ temp;
						hMH->neighbors->reserved = (uint8_t)*temp;
						++temp;
						hMH->neighbors->linkMessageSize = (uint16_t)*temp;
						temp += 2;
				
						uint16_t nb = (hMH->neighbors->linkMessageSize - 4) / 16;
				
						hMH->neighbors->neighborsAddrList = (in6_addr*)temp;
						hMH->neighbors->sizeNAL = nb;
						temp += 16;
				
						compt+=hMH->neighbors->linkMessageSize;
					}
				} else if (mH->messageType == TC_MESSAGE) {
					tcMessageHeader* tMH;
					tMH->ANSN = (uint16_t)*temp;
					temp += 2;
					tMH->reserved = (uint16_t)*temp;
					temp += 2;
				
					uint16_t nb = (mH->messageSize - 4)/16;
				
					tMH->advertisedNeighborMainAddress = (in6_addr*)temp;
					tMH->sizeANMA = nb;
					temp += 16;
				}
			}
		} catch (std::exception& e) {
			std::cerr << e.what() << std::endl;
    	}
	}
	
	void Listener::receptionMsg(Message msg) {
		this->mListMsg.push_front(msg);
	}
  
	Message Listener::getMsg(){
		this->mListMsg.pop_back();
	}
}

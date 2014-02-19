#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>

using boost::asio::ip::udp;

int SendPacket(struct paquet,struct message,struct TCHBody,in6_addr dest_ip,char type) //message est la structure
{
	try
	{
		boost::asio::io_service io_service;

		//udp::socket socket(io_service, udp::endpoint(udp::v6(), 7171)); // (1)
		udp::socket socket(io_service, udp::endpoint(udp::v6(), dest_ip)); // (1)

		for (;;)
    		{
      			boost::array<char, 1> recv_buf;
      			udp::endpoint remote_endpoint;
      			boost::system::error_code error;
      			socket.receive_from(boost::asio::buffer(recv_buf),
          		remote_endpoint, 0, error);

      			if (error && error != boost::asio::error::message_size)
        		throw boost::system::system_error(error);
			

			class packet{
				public struct paquet;
				public struct message;
				public struct TCHBody;


				friend ostream& operator<< (ostream& out, packet& pack) {
				out << pack.paquet << " " << pack.message<< " " << pack.TCHBody;   //The space (" ") is necessari for separete elements
				return out;
				}

				/*friend istream& operator>> (istream& in, Object& object) {
				in >> object.a;
				in >> object.b;
				return in;
				}*/

			}




 
	/*std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> convert;
	std::string converted = convert.to_bytes(u"HELLO, WORLD!");
	const char *bytes = converted.data();
      std::string message;// = "Bienvenue sur le serveur ! Mode non connecté.\n";
	  std::cin >> message;*/


	std::packet pac;
	stringstream ss;
	ss << pac;

	/*std::struct Type_Mess;
	if(type == 2)
	{
		Type_Mess 			
      std::char* message;
	std::struct Mess
	{
		messageHeader message;
		packetHeader packet;
	};*/

		/*uint16_t packetLength;
  		uint16_t packetSequenceNumber;

		uint8_t messageType;
  		uint8_t vTime;
  		uint16_t messageSize;
  		in6_addr originatorAddress;
  		uint8_t timeToLive;
  		uint8_t hopCount;
  		uint16_t messageSequenceNumber;*/
	
      	boost::system::error_code ignored_error;
	socket.send_to(boost::asio::buffer(ss.str().c_str()),
          remote_endpoint, 0, ignored_error);
      	/*socket.send_to(boost::asio::buffer(message),
          remote_endpoint, 0, ignored_error);*/
    }
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}

	return 0;
}

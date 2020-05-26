#include "..//Header files//NetworkHandler.h"

void NetworkHandler::SendPacket(sf::TcpSocket socket, sf::Packet packet)
{
	socket.send(packet);
}

void NetworkHandler::RecievePacket(sf::TcpSocket socket, sf::Packet packet)
{
	socket.receive(packet);
}

NetworkException::NetworkException(std::string exception_msg)
{
	exception_message = exception_msg;
}

const char* NetworkException::what() const throw()
{
	return exception_message.c_str();
}

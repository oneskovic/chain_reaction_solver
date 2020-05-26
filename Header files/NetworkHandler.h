#pragma once
#include <SFML/Network.hpp>
#include <string>
#include <exception>
class NetworkHandler
{
public:
	void SendPacket(sf::TcpSocket socket, sf::Packet packet);
	void RecievePacket(sf::TcpSocket socket, sf::Packet packet);
private:
};
class NetworkException :  std::exception
{
private:
	std::string exception_message;
public:
	NetworkException(std::string exception_msg);
	virtual const char* what() const throw();
};
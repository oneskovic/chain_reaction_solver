#pragma once
#include <thread>
#include <iostream>
#include <vector>
#include <list>
#include <unordered_map>
#include "SFML/Network.hpp"
#include "GameBoard.h"
#include "NetworkCodes.h"
class Room
{
public:
	Room();
	std::vector<sf::TcpSocket*> clients;
	std::vector<int> figures;
	GameBoard board;
};
class Server
{
public:
	Server();
private:
	bool is_running;
	void AcceptClients();
	void HandleRoom(Room* room);
	void ResolveRequest(sf::TcpSocket* client_socket);
	std::list<sf::TcpSocket*> clients;
	std::unordered_map<std::string, Room> rooms;
};

#include "../Header files/Server.h"
#include <random>

Room::Room()
{
	clients.reserve(2);
	figures.reserve(2);
	board = GameBoard();
}


Server::Server()
{
	is_running = true;
	std::thread accepter = std::thread(&Server::AcceptClients, this);
	while (is_running)
	{
		int input; std::cin >> input;
		if (input == -1)
		{
			is_running = false;
		}
	}
	accepter.join();
}

void Server::AcceptClients()
{
	sf::TcpListener listener;
	if (listener.listen(12000) != sf::Socket::Done)
	{
		// errooo
	}

	while (is_running)
	{
		sf::TcpSocket socket;
		clients.push_back(&socket);
		if (listener.accept(socket) != sf::Socket::Done)
		{
			//errrro
		}
		else
		{
			ResolveRequest(&socket);
		}
	}
}

void Server::HandleRoom(Room* room)
{
	int player_to_move = 0;
	while (!room->board.IsFinished())
	{
		sf::Packet packet;
		try
		{
			room->clients[player_to_move]->receive(packet);
		}
		catch (std::exception e)
		{
			std::cout << e.what();
		}
		room->clients[(player_to_move + 1) % 2]->send(packet);
	}
}

void Server::ResolveRequest(sf::TcpSocket* client_socket)
{
	sf::Packet request, response;
	client_socket->receive(request);
	int request_type; request >> request_type;

	if (request_type == REQUEST_JOINGAMEID)
	{
		std::string game_id;
		request >> game_id;
		rooms[game_id].clients.push_back(client_socket);
		if (rooms[game_id].clients.size() == 1)
		{
			response << REQUEST_WAIT;
			client_socket->send(response);
		}
		else
		{
			response << REQUEST_START;
			client_socket->send(response);
			rooms[game_id].clients[0]->send(response);
			std::thread room_handler(&Server::HandleRoom, this, &rooms[game_id]);
		}
	}
	else if (request_type)
	{

	}

}

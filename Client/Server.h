#pragma once
#include <thread>
#include <chrono>
#include "Client.h"

#define PORTNUM 54000


class Server
{
private:
	vector <string> fileName;
	Socket serverSocket;
	vector <string> onlineConnection;
public:
	Socket GetSocket() {
		return serverSocket;
	}

	/*
		function create socket and tell winsock the socket is listenning
		
		at the port PORTNUM
	*/
	void Create() {
		serverSocket.Initialize(PORTNUM);
		
		serverSocket.Bind();

		serverSocket.Listen();
	}

	bool SignInHandle(string);
	bool SignUpHandle(string,string);

	//void HandleConnection(SOCKET);

	//void Listenning();

	int ReceiveFile(const SOCKET& client);

	void Shutdown() {
		serverSocket.Disconnect();
	}

	Server() {	};
	~Server() {};
	
	
};


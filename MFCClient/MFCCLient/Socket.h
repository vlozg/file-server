#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma once
#include <iostream>
#include <string>
#include <WS2tcpip.h>
#include <conio.h>
#include <thread>
#include <stdint.h>
#pragma comment(lib, "ws2_32.lib")

typedef struct sockaddr_in SA_IN;
typedef struct sockaddr SA;

#define BUFFER_SIZE 4096

using namespace std;

class Socket
{
private:
	SOCKET sock;
	SA_IN hint;
public: 
	Socket() {
		hint.sin_family = AF_INET;
		hint.sin_port = htons(54000);
		hint.sin_addr.s_addr = htonl(INADDR_ANY);
	};

	~Socket() {};

	SOCKET GetSock() {return sock;}
	SA_IN GetSA_IN() {return hint;}

	// initialize socket
	void Initialize(int port) {
		// create a socket with TCP protocol
		sock = socket(AF_INET, SOCK_STREAM, 0);
		if (sock == SOCKET_ERROR) {
			Error("Create socket failed: %d\n");
		}
		// Fill in a hint structure
		hint.sin_family = AF_INET;
		hint.sin_port = htons(port);
		hint.sin_addr.S_un.S_addr = INADDR_ANY;
	}

	//bind socket
	void Bind() {
		int retCode = bind(sock, (SA*)&hint, sizeof(hint));
		if (retCode == SOCKET_ERROR)
			Error("Bind failed: %d\n");
	}

	//tell winsock that socket is for listenning
	void Listen() {
		int retCode = listen(sock, SOMAXCONN);
		if (retCode == SOCKET_ERROR)
			Error("Listen failed: %d\n");
	}
	// accept a connection
	SOCKET Accpet(Socket connection) { 
		int connectionSize = sizeof(connection.GetSA_IN());
		SOCKET socket = accept(sock, (sockaddr*)&connection.GetSA_IN(), &connectionSize);
		if (socket == INVALID_SOCKET) {
			Error("Accept Fail !!!\n");
		}
		return socket;
	}

	// connect to a socket
	int Connect(string ipAddress) {
		// change to the ip want to connect
		inet_pton(AF_INET, ipAddress.c_str(), &hint.sin_addr);
		
		return connect(sock, (SA*)&hint, sizeof(hint));
	}
	// disconnect socket
	void Disconnect() {closesocket(sock);}

	void Error(const char* err) {
		ErrorExit(err, WSAGetLastError());
	}

	/*
	Function that handle errors when the server operate
	*/
	void ErrorExit(const char* format, ...)
	{
		va_list	args;

		va_start(args, format);
		string err;
		err.append(format);
		cout << err << endl;
		va_end(args);
		WSACleanup();
		system("pause");
		exit(1);
	}
};

int Send(SOCKET receiver, const char* buffer, int32_t size, int flag);
int Send_s(SOCKET receiver,const string& buffer, int flag);
int Recv(SOCKET sender, char* buffer, int32_t size, int flag);


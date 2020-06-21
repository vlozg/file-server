#pragma once
#include "Socket.h"
#include <vector>
#include <fstream>
#include <sstream>

using namespace std;


class Client
{
private:
	string username;
	string password;
	Socket clientSocket;
public:
	vector <string> fileName;
	
	// declare in Client.cpp
	bool Connect(string ipAddress);

	bool SignUp(string, string);

	bool SignIn(string, string);

	int SendFileToServer();

	int GetFileFromServer();

	void InputFileToGet(string& fileName, string& dir);


	void HandleNoti(string log) {
		// log the noti 
		cout << log << endl;
	}
	int GetFile(string& fileName, const string& dir);

	bool CheckReceive(string& res, int& size) {
		char buffer[BUFFER_SIZE];
		int byteReceived = recv(clientSocket.GetSock(), buffer, BUFFER_SIZE, 0);
		size = byteReceived - 1;
		res = buffer + 1;
		if (buffer[0] == 0) {
			return true; // not noti
		}
		else {
			HandleNoti(res);
			return false;
		}
	}

	Socket GetSocket() {
		return clientSocket;
	}

	void SetSocket(Socket sock) {
		clientSocket = sock;
	}

	void SetSOCKET(SOCKET sock) {
		Socket newS;
		newS.SetSocket(sock);
		this->SetSocket(newS);
	}

	string GetUsername() {
		return username;
	}
	string GetPassword() {
		return password;
	}
	void SetUsername(string newUsername) {
		username = newUsername;
	}
	void SetPassword(string newPassword) {
		password = newPassword;
	}

	Socket getClientSocket() {
		return clientSocket;
	}

	void Disconnect() {
		clientSocket.Disconnect();
	}
	
	void Create(int port) {
		clientSocket.Initialize(port);
	}

	Client() {};
	~Client() {};
};


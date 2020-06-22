#pragma once
#include <vector>
#include <fstream>
#include <sstream>
#include "CMenu.h"


class Client
{
private:
	string username;
	string password;
	Socket clientSocket;
	bool isConnected;
public:
	vector <string> fileName;
	CMenu UI;
	// declare in Client.cpp
	bool Connect(string ipAddress);
	bool SignUp();
	bool SignIn();
	int SendFileToServer();
	int GetFileFromServer();
	void InputFileToGet(string& fileName, string& dir);

	bool IsConnected() {
		return isConnected;
	}

	int GetFile(string& fileName, const string& dir);
	int SendFile(const SOCKET& freceiver, const string& dir);

	bool CheckReceive(string &res,int &size,bool isString = TRUE,char *buf = NULL) {
		char buffer[BUFFER_SIZE];
		int byteReceived = Recv(clientSocket.GetSock(), buffer, BUFFER_SIZE, 0);
		if (byteReceived == SOCKET_ERROR) {
			isConnected = false;
			return true;
		}
		size = byteReceived - 1;
		res = buffer + 1;
		if (!isString) 
			memcpy(buf,buffer+1,size);
		cout << size << endl;
		if (buffer[0] == '0') {
			return true; // not noti
		}
		else {
			UI.drawNotification(res);
			return false;
		}
	}

	string GetUsername() {
		return username;
	}
	string GetPassword() {
		return password;
	}
	Socket GetSocket() {
		return clientSocket;
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
		string noti = "Disconnect";
		Send(clientSocket.GetSock(), noti.c_str(), noti.length() + 1, 0);
		clientSocket.Disconnect();
	}
	
	void Create(int port) {
		
		clientSocket.Initialize(port);
	}

	void UIReset() {
		UI.resetActivity();
		system("CLS");
		UI.Initialize();
	}

	Client() {};
	~Client() {};
};


#pragma once
#include <vector>
#include <fstream>
#include <sstream>
#include <limits>
#include <sys/types.h>
#include <sys/stat.h>
#include <condition_variable>
#include "CMenu.h"


class Client
{
private:
	string username;
	string password;
	Socket clientSocket;
	bool isConnected;
	
	mutex mutex_;
	condition_variable covaNoti, covaRecv;
	bool notiHandle = false;	//Flag, true mean allow 2nd thread peek every packet
	bool isNotiListenOn = false;
	bool isUpload = true; //flag

public:
	vector <string> fileName;
	CMenu UI;
	// declare in Client.cpp
	bool Connect(string ipAddress);
	bool SignUp();
	bool SignIn();
	int SendFileToServer();
	int GetFileFromServer();
	void NotiHandle();

	bool IsConnected() {
		return isConnected;
	}

	int GetFile(string& fileName, const string& dir);
	int SendFile(const SOCKET& freceiver, const string& dir);

	//Special Recv function that filter out every noti packet
	int Recv_NonNoti(char* buffer, int32_t size, int flag) {
		char tbuffer[BUFFER_SIZE];
		int bytesReceived = 0;

		{
			//cout << "Recv wait\n";
			unique_lock<mutex> lck(mutex_);
			covaRecv.wait(lck, [this] { return !(notiHandle && isNotiListenOn); });
			//cout << "Recv awake\n";
			bytesReceived = Recv(clientSocket.GetSock(), tbuffer, BUFFER_SIZE, flag);
			notiHandle = true;
		}
		//cout << "R";
		covaNoti.notify_one();
		if (bytesReceived == SOCKET_ERROR) return bytesReceived;

		//Remove header (post-processing)
		size = bytesReceived - 1;
		memcpy(buffer,tbuffer+1,size);
		return size;
	}

	//Special Recv function that filter out every noti packet
	int Recv_NonNoti(string& buffer, int flag) {
		int32_t size;
		char tbuffer[BUFFER_SIZE];
		int bytesReceived = 0;

		{
			//cout << "Recv wait\n";
			unique_lock<mutex> lck(mutex_);
			covaRecv.wait(lck, [this] { return !(notiHandle && isNotiListenOn); });
			//cout << "Recv awake\n";
			bytesReceived = Recv(clientSocket.GetSock(), tbuffer, BUFFER_SIZE, flag);
			notiHandle = true;
		}
		//cout << "R";
		covaNoti.notify_one();
		if (bytesReceived == SOCKET_ERROR) return bytesReceived;

		//Remove header
		size = bytesReceived - 1;
		buffer = (tbuffer + 1);
		return size;
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

	void ServerShutdown() {
		UI.drawNotification("SERVER SHUTDOWN !!! Quitting in");
		UI.drawNotification("3...");
		Sleep(1000);
		UI.drawNotification("2...");
		Sleep(1000);
		UI.drawNotification("1...");
		Sleep(1000);
		exit(1);
	}

	Client() {};
	~Client() {};
};

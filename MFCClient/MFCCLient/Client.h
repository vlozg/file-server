#pragma once
#include <vector>
#include <fstream>
#include <sstream>
#include <limits>
#include <sys/types.h>
#include <sys/stat.h>
#include "CMenu.h"
#include "CMFCMainDlg.h"


class Client
{
private:
	string username;
	string password;
	Socket clientSocket;
	bool isConnected;
	bool isRecv = false;	//Flag
	bool notiHandle = false;	//Flag
	bool isUpload = true; //flag
	string IPAddress;
	int port;
public:
	vector <string> fileName;
	CMenu1 UI;
	CMFCMainDlg* mainDlg;
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
		isRecv = true;

	Peek:
		//Peek into packet
		do
		{
			bytesReceived = recv(clientSocket.GetSock(), tbuffer, BUFFER_SIZE, MSG_PEEK);
			if (bytesReceived == SOCKET_ERROR)
			{
				return bytesReceived;
			}		
		} while (bytesReceived < (sizeof(int32_t) + 1));	//Only accept if peek enough bytes to check
			
		//Check for noti flag
		if (tbuffer[sizeof(int32_t)] == '1')
		{
			notiHandle = true;

			//Loop until NotiHandle completed and set notiHandle flag to false
			while (notiHandle);
			
			//Go back and peek again
			goto Peek;
		}

		//Get packet
		bytesReceived = Recv(clientSocket.GetSock(), tbuffer, BUFFER_SIZE, flag);
		if (bytesReceived == SOCKET_ERROR) return bytesReceived;
		
		//Remove header
		size = bytesReceived - 1;
		memcpy(buffer,tbuffer+1,size);
		isRecv = false;
		return size;
	}

	//Special Recv function that filter out every noti packet
	int Recv_NonNoti(string& buffer, int flag) {
		int32_t size;
		char tbuffer[BUFFER_SIZE];
		int bytesReceived = 0;

		//Only peek until received non-noti packet
		do
		{
			//Peek into every packet
			do
			{
				bytesReceived = recv(clientSocket.GetSock(), tbuffer, BUFFER_SIZE, MSG_PEEK);
				if (bytesReceived == SOCKET_ERROR)
				{
					return bytesReceived;
				}
			} while (bytesReceived < (sizeof(int32_t) + 1));	//Only accept if peek enough bytes to check
		} while (tbuffer[sizeof(int32_t)] == '1');	//Check for noti flag

		//Get packet
		bytesReceived = Recv(clientSocket.GetSock(), tbuffer, BUFFER_SIZE, flag);
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

	void SetIPAddress(string newIP) {
		IPAddress = newIP;
	}

	string GetIPAddress() {
		return IPAddress;
	}

	int GetPort() {
		return port;
	}

	void SetPort(int newPort) {
		port = newPort;
	}

	Client() {};
	~Client() {};
};


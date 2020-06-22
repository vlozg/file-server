#pragma once

#include <thread>
#include <chrono>
#include "Client.h"


#define PORTNUM 54000
#define FILE_DB "FileDB.txt"


class Server
{
private:
	vector <string> fileName;
	Socket serverSocket;
	vector <Client> onlineConnection;
public:
	CMainWindow c;
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

	void SignIn(Client&);
	void SignUp(Client&);

	int UserValidation(string,string);
	bool AddUser(string,string);
	int SendFileForServer(const SOCKET& freceiver, const string& dir);
	int GetFile(const SOCKET& fsender, string& fileName, const string& dir);
	
	/*
	Add new file to database
	Parameter:
	-	fileName
	*/
	void UpdateDB(const string& fileName)
	{
		ofstream out(FILE_DB, ios::app);
		out << fileName << "\n";
	}

	int GetFileFromClient(Client&);

	vector<string> InputFileToSend(const SOCKET& client);
	int SendFileToClient(Client &client);

	void SendNoti(string noti) {
		noti = "1" + noti;
		for (int i = 0; i < onlineConnection.size(); i++) {
			Send_s(onlineConnection[i].GetSocket().GetSock(),noti, 0);
		}
	}

	void ClientDisconnectLog(Client& client) {
		for (int i = 0; i < onlineConnection.size(); i++) {
			if (onlineConnection[i].GetUsername() == client.GetUsername()) {
				onlineConnection.erase(onlineConnection.begin() + i, onlineConnection.begin() + i +1);
				break;
			}
		}
		c.drawLog(client.GetUsername() + " log off !!!\n");
		c.resetCursorListOnlineUser();
		listOnlineUser();
		SendNoti(client.GetUsername() + " log off !!!");
	}
	
	void listOnlineUser() {
		c.resetCursorListOnlineUser();
		for (int i = 0; i < onlineConnection.size(); i++) {
			c.drawListOnlineUsers(onlineConnection[i].GetUsername());
		}
	}


	void Shutdown() {
		serverSocket.Disconnect();
	}

	Server() {	};
	~Server() {
		SendNoti("Server down !!! Disconect all client !!!");
		Shutdown();
	};
};


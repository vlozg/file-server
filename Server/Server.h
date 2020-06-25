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
	bool isUploading = false;
	string uploadClient;
public:
	CMainWindow c;
	Socket GetSocket() {
		return serverSocket;
	}
	bool isUpload() {
		return isUploading;
	}
	void SetUploadClient(string name) {
		uploadClient = name;
	}
	/*
		function create socket and tell winsock the socket is listenning

		at the port PORTNUM
	*/
	void Create() {
		serverSocket.Initialize(PORTNUM);

		serverSocket.Bind();

		serverSocket.Listen();

		DataBaseScan();
	}

	bool SignIn(Client&);
	void SignUp(Client&);

	int UserValidation(string, string);
	bool AddUser(string, string);
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
	int SendFileToClient(Client& client);

	void SendNoti(string noti, string name = "") {
		noti = "1" + noti;
		for (int i = 0; i < onlineConnection.size(); i++) {
			if (name == onlineConnection[i].GetUsername())
				continue;
			Send_s(onlineConnection[i].GetSocket().GetSock(), noti, 0);
		}
	}

	void ClientDisconnectLog(Client& client) {
		for (int i = 0; i < onlineConnection.size(); i++) {
			if (onlineConnection[i].GetUsername() == client.GetUsername()) {
				onlineConnection.erase(onlineConnection.begin() + i, onlineConnection.begin() + i + 1);
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

	void DataBaseScan() {
		string file;
		ifstream db(FILE_DB); //open database
		// open temp file
		ofstream temp("temp.txt");
		while (getline(db, file)) {
			ifstream f(file);
			//write the name to temp if file exists
			if (f.good()) {
				temp << file << endl;
			}
		}
		temp.close();
		db.close();
		remove(FILE_DB);
		rename("temp.txt", FILE_DB);
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


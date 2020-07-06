#pragma once
#include <vector>
#include <fstream>
#include <sstream>
#include <limits>
#include <sys/types.h>
#include <sys/stat.h>
#include <condition_variable>
#include "Socket.h"

using namespace std;

class Client
{
private:
	string username;
	Socket clientSocket;
	bool isConnected = false;

	thread* notiThread;
	mutex mutex_;
	condition_variable covaNoti, covaRecv;
	//Flags, true mean allow 2nd thread peek and process every packet
	bool notiHandle = false;
	bool isNotiListenOn = false;
	void(*pPrintNoti) (string) = NULL;

	string lastError;

public:

	Client() {};
	~Client() {};
	Socket GetClientSocket() { return clientSocket; }
	string GetLastError() { return lastError; }
	void SetError(string err) { lastError = err; }
	void SetError(int err) { lastError = to_string(err); }
	void SocketError() {
		isConnected = false;
		SetError(WSAGetLastError());
	}

	void Create(int port) { clientSocket.Initialize(port); }
	bool Connect(string ipAddress, int port);
	void Disconnect();
	bool IsConnected() { return isConnected; }

	bool SignUp(string& name, string& pass);
	bool SignIn(string& name, string& pass);
	void SignOut();
	string GetUsername() { return username; }
	Socket GetSocket() { return clientSocket; }

	int UploadCall() {
		//Send flag to server
		if (Send_s(clientSocket.GetSock(), "Upload", 0) == SOCKET_ERROR)
		{
			SocketError();
			return -1;
		}
		return 1;
	}
	int DownloadCall() {
		//Send flag to server
		if (Send_s(clientSocket.GetSock(), "Download", 0) == SOCKET_ERROR)
		{
			SocketError();
			return -1;
		}
		return 1;
	}

	int GetFile(string& fileName, const string& dir);
	int SendFile(const SOCKET& freceiver, const string& dir);

	void NotiHandle();
	void SetPrintNotiFunction(void(*f)(string)) { pPrintNoti = f; }
	void TurnOffNotiHandle() { isNotiListenOn = false; }
	int Recv_NonNoti(char* buffer, int32_t size, int flag);
	int Recv_NonNoti(string& buffer, int flag);
};

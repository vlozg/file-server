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
	int DownloadCall(string& db) {
		//Send flag to server
		if (Send_s(clientSocket.GetSock(), "Download", 0) == SOCKET_ERROR)
		{
			SocketError();
			return -1;
		}

		GetFile(db, "");
		return 1;
	}

	int GetFile(string& fileName, const string& dir);
	int SendFile(const SOCKET& freceiver, const string& dir);

	/*
	This function check every package and handle if it's a notification
	*/
	template <class T>
	void NotiHandle(T* p, void(T::*pFunc)(string)) {
		char buffer[BUFFER_SIZE];
		int bytesReceived = 0;

		while (username.length() == 0);

		isNotiListenOn = true;
		while (isNotiListenOn)
		{
			//cout << "Noti wait\n";
			unique_lock<mutex> lck(mutex_);
			covaNoti.wait(lck, [this] { return (notiHandle); });
			//cout << "Noti awake\n";

			//Peek into every packet
			//Only accept if peek enough bytes to check
			do
			{
				bytesReceived = recv(clientSocket.GetSock(), buffer, BUFFER_SIZE, MSG_PEEK);

				if (bytesReceived == SOCKET_ERROR)
					goto ErrorOccur;

			} while (bytesReceived < (sizeof(int32_t) + 1));

			if (buffer[sizeof(int32_t)] != '1')	//Check for noti flag
			{
				notiHandle = false;
				//cout << "N";
				lck.unlock();
				covaRecv.notify_one();	//Notify when detect data packet
				continue;
			}

			bytesReceived = Recv(clientSocket.GetSock(), buffer, BUFFER_SIZE, 0);

			if (bytesReceived == SOCKET_ERROR)
				goto ErrorOccur;

			lck.unlock();

			//Call function that print notification
			if (pFunc != NULL && p != NULL)
				(p->*pFunc)(buffer + 1);
		}

		notiHandle = false;
		return;

	ErrorOccur:
		notiHandle = false;
		isNotiListenOn = false;
		SocketError();
		return;
	}

	template <class T>
	void TurnOnNotiHandle(T* p, void(T::* pFunc)(string)) { notiThread = new thread(&NotiHandle<T>, this, p, pFunc); }
	void TurnOffNotiHandle() { 
		isNotiListenOn = false;
		notiThread->join();
		delete notiThread; 
	}
	int Recv_NonNoti(char* buffer, int32_t size, int flag);
	int Recv_NonNoti(string& buffer, int flag);
};

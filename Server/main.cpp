#define _WINSOCK_DEPRECATED_NO_WARNINGS
#include "Server.h"

vector <thread> clientHandleThread;

Server myServer;

int HandleFlagSignal(string flag) {
	if (flag.compare("SignIn") == 0) {
		return 1;
	}
	else if (flag.compare("SignUp") == 0) {
		return 2;
	}
	else if (flag.compare("Upload") == 0) {
		return 3;
	}
	else if (flag.compare("Download") == 0) {
		return 4;
	}
	else if (flag.compare("Disconnect") == 0) {
		return 5;
	}
	//other flag
	return 6;
}


void HandleConnection(SOCKET clientSocket) {
	char buffer[BUFFER_SIZE];
	Socket serverSocket = myServer.GetSocket();

	Client client;
	client.SetSOCKET(clientSocket);

	bool clientDisconnect = false;
	bool signIn = false;

	while (1) {
		ZeroMemory(buffer, BUFFER_SIZE);

		// wait for client to send data
		int bytesReceived = Recv(clientSocket, buffer, BUFFER_SIZE, 0);
		if (bytesReceived == SOCKET_ERROR) {
			if (signIn)
				myServer.ClientDisconnectLog(client);
			return;
		}

		string notification;
		notification.append(buffer);		//convert char* to string

		switch (HandleFlagSignal(notification)) {
		case 1:
			if (myServer.SignIn(client)){ signIn = true; }
			break;
		case 2:
			myServer.SignUp(client);
			break;
		case 3:
			myServer.GetFileFromClient(client);
			break;
		case 4:
			myServer.SendFileToClient(client);
			break;
		case 5:
			myServer.ClientDisconnectLog(client);
			clientDisconnect = true;
			break;
		default:
			myServer.ClientDisconnectLog(client);
			clientDisconnect = true;
			break;
		}
		if (clientDisconnect) {
			return;
		}
	}

}


/*
	function execute server listenning and
	starting accept connection
*/
void ServerListenning(Server server) {
	Socket clientSock;
	clientSock.Initialize(PORTNUM);
	SOCKET newClient;
	Socket serverSocket = server.GetSocket();

	while (true) {
		if (clientHandleThread.size() == 0) {
			myServer.c.drawLog("Waiting for a connection !!!\n");
		}

		newClient = serverSocket.Accpet(clientSock);
		if (newClient == SOCKET_ERROR) {
			myServer.c.drawLog("Failed to accept a connection !!!\n");
		}
		else {

			//push a new thread into the array to handle new connection
			//after that comeback and listen to new connection
			clientHandleThread.push_back(thread(HandleConnection, newClient));

		}
	}
}

int main()
{
	//initialize winsock
	WSADATA wsData;
	WORD wVersionRequested = MAKEWORD(2, 2);// version
	int wsOk = WSAStartup(wVersionRequested, &wsData);
	if (wsOk != 0) {
		cerr << "Start up failed!! Quitting" << endl;;
		return 0;
	}

	if (LOBYTE(wsData.wVersion) != LOBYTE(wVersionRequested) ||
		HIBYTE(wsData.wVersion) != HIBYTE(wVersionRequested))
	{
		cout << "Supported version is too low" << endl;
		WSACleanup();
		return 0;
	}

	/*myHostInfo serverInfo;
	cout << "Host Name: " << serverInfo.getHostName() << endl;
	cout << "Host IP Address: " << serverInfo.getHostIPAddress() << endl;*/

	/*
		server side
	*/

	//create socket server
	myServer.Create();

	//start listenning and accepting connection
	ServerListenning(myServer);

	// close server
	myServer.Shutdown();
	
	// join all the thread
	for (int i = 0; i < clientHandleThread.size(); i++){
		clientHandleThread[i].join();
	}
	return 0;
}



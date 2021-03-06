#include "Client.h"


void main()
{
	string ipAddress;			// IP Address of the server
	int port;						// Listening port # on the server

	// Initialize WinSock
	WSAData data;
	WORD ver = MAKEWORD(2, 2);
	int wsResult = WSAStartup(ver, &data);
	if (wsResult != 0)
	{
		cerr << "Can't start Winsock, Err #" << wsResult << endl;
		return;
	}

	//Initialize client socket connection (login not included)
	Client client;
	bool LogIn = true;
	int clientChoice;

	while (1) {
		//get server address and port
	changeIP:
		cout << "Server IP Address : ";
		cin >> ipAddress;
		cout << "Server port: ";
		cin >> port;
	reconnect:
		int rec;
		client.Create(port);
		if (!client.Connect(ipAddress)) {
			std::cout << "Server is not currently available !!!\n Do you want to reconnect ? (1: yes, 2: Change IP): ";

		retype:
			std::cin >> rec;
			switch (rec) {
			case 2:
				goto changeIP;
				break;
			case 1:
				goto reconnect;
				break;
			default:
				std::cout << "Wrong input!!! Type again" << endl;
				goto retype;
				break;
			}
		}

		if (!client.IsConnected())
			goto reconnect;
		else
			break;
	}
	thread NotiListen(&Client::NotiHandle, &client);
	//Main loop
	while (1) {
		if (!LogIn) {
			client.Create(port);
			client.Connect(ipAddress);
			LogIn = true;
			NotiListen = thread(&Client::NotiHandle, &client);
		}
		if (!client.IsConnected())
			goto reconnect;
		cout << "1. Upload a file to server !!! " << endl;
		cout << "2. Download a file from server !!! " << endl;
		cout << "3. Sign Out !!! " << endl;
		cout << "4. Exit!!!" << endl;
		cout << "Your choice: ";
		cin >> clientChoice;

		switch (clientChoice) {
		case 1:
			client.SendFileToServer();
			break;
		case 2:
			client.GetFileFromServer();
			break;
		case 3:
			client.Disconnect();
			LogIn = false;
			NotiListen.join();
			continue;
			break;
		case 4:
			cout << "Thanks for connecting !!! Bye !!!" << endl;
			Sleep(1000);
			exit(1);
			break;
		default:
			cout << "Input is not valid !!! Try again in 1s !!!" << endl;
			client.UI.resetActivity();
			continue;
		}
		if (!client.IsConnected())
			goto reconnect;
		cout << "Do u want to continue ? (1: yes, 2: no): ";
		cin >> clientChoice;
		switch (clientChoice) {
		case 1:
			client.UI.resetActivity();
			continue;
		case 2:
			// Gracefully close down everything
			client.Disconnect();
			NotiListen.join();
			exit(1);
		}
	}
	
}
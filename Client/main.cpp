#include "Client.h"

void main()
{
	string ipAddress = "192.168.31.192";			// IP Address of the server
	int port = 54000;						// Listening port # on the server

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
	client.Create(port);
	if (!client.Connect(ipAddress)) {
		cout << "Quitting !!! " << endl;
	}

	//Main loop
	while (1) {
		if (!LogIn) {
			client.Create(port);
			client.Connect(ipAddress);
			LogIn = true;
		}
		
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
			continue;
			break;
		case 4:
			cout << "Thanks for connecting !!! Bye !!!" << endl;
			Sleep(1000);
			exit(1);
			break;
		default:
			cout << "Input is not valid !!! Try again in 1s !!!" << endl;
			system("clrscr");
			continue;
		}
		cout << "Do u want to continue ? (1: yes, 2: no): ";
		cin >> clientChoice;
		switch (clientChoice) {
		case 1:
			system("clrscr");
			continue;
		case 2:
			// Gracefully close down everything
			client.Disconnect();
			return;
		}
	}
	
}

#include "Client.h"

/*
	function execute client connect to server

	return false if the connection fail
*/
bool Client::Connect(string ipAddress) {
	if (clientSocket.Connect(ipAddress) == SOCKET_ERROR)
	{
		clientSocket.Error("Connect fail!");
		Disconnect();
		WSACleanup();
		return false;
	}
	return true;
}

/*
	function send Sign In request to server
*/
bool Client::SignIn(string username, string password) {
	string notification = "SignIn";
	do {
		//Send sign in notification to Server
		Send_s(clientSocket.GetSock(), notification, 0);

		//send account information to Server
		int sendResultUsername = Send_s(clientSocket.GetSock(), username, 0);
		int sendResultPassword = Send_s(clientSocket.GetSock(), password, 0);
		if (sendResultUsername != SOCKET_ERROR && sendResultPassword != SOCKET_ERROR) {

			string signInRes;
			int size;
			while (!CheckReceive(signInRes, size));

			if (signInRes[0] == '1') {
				return true;		// Breakout the loop
			}
			else {
				return false;
			}
		}
	} while (1);
	return false;
}

/*
	function send Sign Up request to server
*/
bool Client::SignUp(string username, string password) {
	string notification = "SignUp";
	do {
		Send_s(clientSocket.GetSock(), notification, 0);

		//Append string
		int sendResult = Send_s(clientSocket.GetSock(), username, 0);		//Send sign up notification to Server
		sendResult = Send_s(clientSocket.GetSock(), password, 0);
		if (sendResult != SOCKET_ERROR) {

			string signUpRes;
			int size;
			while (!CheckReceive(signUpRes, size));

			if (signUpRes[0] == '1') {
				return true;
			}
			else {
				return false;
			}
		}
	} while (1);

}

/*
The UI that prompt user to input the file name
Return: processed file directory get from user
*/
string InputFileToSend()
{
	string fileDir;
	cout << "Type in file you want to upload:\n"
		<< "Tip: you can drag and drop the file you want into here.\n"
		<< "> ";
	getline(cin, fileDir);

	//Delete the " symbol in case the user drag n drop file into console
	if (*fileDir.begin() == '\"')
		fileDir.erase(fileDir.begin());
	if (*(fileDir.end() - 1) == '\"')
		fileDir.erase(fileDir.end() - 1);

	return fileDir;
}

/*
Control uploading process from client to server (client side)
(include calling UI function and process function)
Parameter:
-	server: server's socket

Return:
- 1: successful
- 0:
- -1: connection error
*/
int Client::SendFileToServer()
{
	string fileDir;
	// send signal flag to server
	string notification = "Upload";
	Send_s(clientSocket.GetSock(), notification, 0);
	fileDir = InputFileToSend();

	//Get file dir & art file transfering
	int err = SendFile(clientSocket.GetSock(), fileDir);

	switch (1)
	{
	case 1:
		cout << "Upload successful!\n";
		return 1;
	case -1:
		return -1;
	case -2:
		break;
	default:
		break;
	}

	return 0;
}

/*
Control downloading process from server to client (client side)
(include calling UI function and process function)
Parameter:
-	client: client's socket
Return:
- 1: successful
- 0: client disconnected
- -1: connection error
*/
int Client::GetFileFromServer()
{
	char buffer[BUFFER_SIZE];
	string dir;
	string fileDB, input;

	string notification = "Download";
	Send_s(clientSocket.GetSock(), notification, 0);


	//Get file database from server
	GetFile(fileDB, "");
	fileDB = fileDB;
	ifstream db(fileDB);

	//Get wanted files
	vector<string> choices;
	cout << "Choose file to download:\n"
		<< "Tip: you can enter multiple choices with each choice seperated\n"
		<< "from the orthers by a space.\n";

	string temp;
	while (getline(db, temp))
	{
		choices.push_back(temp);
		cout << choices.size() << ". " << temp << "\n";
	}
	cout << "> ";
	cin >> ws;
	getline(cin, input);

	//Get wanted diractory
	cout << "\nType in the directory you want your file to be saved in:\n"
		<< "Precaution: this apply to all the file you choose.\n"
		<< "Tip: leave the input empty to save to the execution file directory\n"
		<< "> ";
	cin >> ws;
	getline(cin, dir);

	//List all the wanted files and send to server
	stringstream userChoices(input);
	int i = 0, index;
	do
	{
		userChoices >> index;
		buffer[i++] = index;
	} while (userChoices.good());
	buffer[i] = '\0';

	int sendResult = Send(clientSocket.GetSock(), buffer, i + 1, 0);
	if (sendResult == SOCKET_ERROR) return -1;

	while (i > 0)
	{
		int err = GetFile(temp, dir);
		switch (err)
		{
		case 1:
			cout << temp << " downloaded successful!\n";
			i--;
			break;
		case -1:
			return -1;
		case -2:
			return -2;
		default:
			break;
		}
	}

	return 1;
}
/*
Getting file process
Precaution:
-   Both SendFile and GetFile must start at the same time
-   This function DOES NOT check for dupplicated file.
	If that happen, that file will be overwritten

Parameter:
-    fsender: sender's socket
-    fileName: received file name
-    dir: desired file directory. By default, file will be saved in the same directory with exe.

Return:
-    1 = successful
-    -1 = connection error
-    -2 = error in the middle of file transfer process
*/
int Client::GetFile(string& fileName, const string& dir)
{

	char buffer[BUFFER_SIZE];
	int bytesReceived;

	//Get file name from sender
	while (1)
	{
		ZeroMemory(buffer, BUFFER_SIZE);

		//Wait for client to send data
		//Loop until file name received

		while (!CheckReceive(fileName, bytesReceived));
	}

	//Get file size from client
	long long int length = 0;
	string data;
	while (1)
	{
		ZeroMemory(buffer, BUFFER_SIZE);

		//Waiting for data
		while (!CheckReceive(data, bytesReceived));

		//Checking data
		if (bytesReceived)
		{
			memcpy(&length, buffer, 8);
			break;
		}
	}

	//Create file
	ofstream out(fileName, ios::trunc | ios::binary);

	//Receiving file loop
	while (length > 0)
	{
		ZeroMemory(buffer, BUFFER_SIZE);
		//Waiting for data
		while (!CheckReceive(data, bytesReceived));

		if (length >= BUFFER_SIZE)
			out.write(buffer, BUFFER_SIZE);
		else
			out.write(buffer, length);
		length -= BUFFER_SIZE;
	}

	return 1;
}
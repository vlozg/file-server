#include "Server.h"

/*
	Handle Sign In request
*/
bool Server::SignIn(Client& client) {
	char buffer[BUFFER_SIZE];
	string username, password;

	Socket clientSocket = client.GetSocket();

	Recv(clientSocket.GetSock(), buffer, BUFFER_SIZE, 0);					//receive account info from client
	username.append(buffer);														//convert char* to string
	Recv(clientSocket.GetSock(), buffer, BUFFER_SIZE, 0);
	password.append(buffer);

	client.SetUsername(username);
	client.SetPassword(password);

	int signInResult = UserValidation(username, password);		//Confirm info

	if (signInResult == 1) {									//send result to client
		SendNoti(username + " log in to server");
		Send(clientSocket.GetSock(), "01", 2, 0);
		onlineConnection.push_back(client);
		c.drawLog(username + " log in to server\n");
		listOnlineUser();
		if (isUploading) {
			SendNoti("F", "", username);
		}
	}
	else {
		Send(clientSocket.GetSock(), "00", 2, 0);
	}
	return signInResult;
}


/*
	Check if user exist in database
*/
int Server::UserValidation(string username, string password) {
	fstream fin;
	int check = 0;
	fin.open("accounts.txt", ios::in);
	while (!fin.eof()) {
		string currUsername, currPassword;
		getline(fin, currUsername, '|');
		getline(fin, currPassword);
		if (username.compare(currUsername) == 0 && password.compare(currPassword) == 0) {
			check = 1;
		}
		else if (username.compare(currUsername) == 0 && password.compare(currPassword) != 0) {			//check username exists or not
			check = -1;
		}
	}
	for (int i = 0; i < onlineConnection.size(); i++)
	{
		if (onlineConnection[i].GetUsername() == username)
		{
			check = 0;
			break;
		}
	}
	fin.close();
	return check;					//check = 1 Sign in success, check = 0 or - 1 sign in fail, check = -1 username exists
}

//Function register new account
bool Server::AddUser(string username, string password) {
	fstream fout;
	int check = UserValidation(username, password);
	if (check == 0) {
		fout.open("accounts.txt", ios::app);
		fout << "\n" << username << "|" << password;
		fout.close();
		return 1;
	}
	return 0;
}

/*
	Handle Sign Up request
*/
void Server::SignUp(Client& client) {
	char buffer[BUFFER_SIZE];
	string newUsername, newPassword;

	Socket clientSocket = client.GetSocket();

	Recv(clientSocket.GetSock(), buffer, BUFFER_SIZE, 0);
	newUsername.append(buffer);
	Recv(clientSocket.GetSock(), buffer, BUFFER_SIZE, 0);
	newPassword.append(buffer);

	client.SetUsername(newUsername);
	client.SetPassword(newPassword);

	bool signUpResult = AddUser(newUsername, newPassword);		//Confirm info
	if (signUpResult == 1) {
		Send(clientSocket.GetSock(), "01", 2, 0);
		c.drawNotification(newUsername + " has just registed !!!\n");
	}
	else {
		Send(clientSocket.GetSock(), "00", 2, 0);
	}


}


/*
Control uploading process from client to server (server side)
(include calling UI function and process function)
Parameter:
-	client: client's socket

Return:
- 1: successful
- 0: client disconnected
- -1: connection error
*/
int Server::GetFileFromClient(Client& client)
{
	isUploading = true;
	string fileName;

	const SOCKET clientSocket = client.GetSocket().GetSock();

	//Start receiving file from client
	int err = GetFile(clientSocket, fileName, "");
	isUploading = false;
	switch (err)
	{
	case 1:
		UpdateDB(fileName);
		c.drawLog(client.GetUsername() + " uploaded file " + fileName + "!!!\n");
		return 1;
	case -1:
		return -1;
	case -2:
		return -2;

	default:
		break;
	}
	return 0;
}

/*
Get name of file client want to download
Parameter
-	client: client's socket
Return: files's name to send to client
*/
vector<string> Server::InputFileToSend(const SOCKET& client)
{
	char buffer[BUFFER_SIZE];
	vector<string> choices, listFile;

	//check database before send to client
	DataBaseScan();
	//Send file database for client to choose
	SendFileForServer(client, FILE_DB);

	//Get files name
	ifstream db(FILE_DB);
	string temp;
	while (getline(db, temp)) listFile.push_back(temp);
	db.close();

	//Get file name from client
	int bytesReceived = Recv(client, buffer, BUFFER_SIZE, 0);
	if (bytesReceived == SOCKET_ERROR) return choices;

	int i = 0;
	while (buffer[i] != '\0')
	{
		choices.push_back(listFile[buffer[i] - 1]);
		i++;
	}

	return choices;
}

/*
Control downloading process from server to client (server side)
(include calling UI function and process function)
Parameter:
-	client: client's socket

Return:
- 1: successful
- 0: client disconnected
- -1: connection error
*/
int Server::SendFileToClient(Client& client)
{
	vector<string> fileDir;
	SOCKET clientSocket;
	clientSocket = client.GetSocket().GetSock();

	//Get file dir & name
	fileDir = InputFileToSend(clientSocket);
	if (fileDir.size() == 0)
		return -1;

	//Start file transfering
	for (int i = 0; i < fileDir.size(); i++)
	{
		int err = SendFileForServer(clientSocket, fileDir[i]);

		switch (err)
		{
		case 1:
			c.drawNotification("File sent succesfully !!!\n");
			c.drawLog(client.GetUsername() + " donwload file " + fileDir[i]);
			break;
		case -1:
			return -1;
		case -2:
			break;
		default:
			break;
		}
	}
	return 1;
}

/*
Getting file process
Precaution:
-   Both SendFileForServer and GetFile must start at the same time
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
int Server::GetFile(const SOCKET& fsender, string& fileName, const string& dir)
{

	char buffer[BUFFER_SIZE];
	int bytesReceived;

	//Get file name from sender
	while (1)
	{
		ZeroMemory(buffer, BUFFER_SIZE);

		//Wait for client to send data
		bytesReceived = Recv(fsender, buffer, BUFFER_SIZE, 0);
		if (bytesReceived == SOCKET_ERROR) return -1;

		//Checking data
		if (bytesReceived > 1)
		{
			fileName = buffer;
			break;
		}

		//Loop until file name received
	}

	//Get file size from client
	long long int length = 0;
	while (1)
	{
		ZeroMemory(buffer, BUFFER_SIZE);

		//Waiting for data
		bytesReceived = Recv(fsender, buffer, BUFFER_SIZE, 0);
		if (bytesReceived == SOCKET_ERROR) return -1;

		//Checking data
		if (bytesReceived)
		{
			memcpy(&length, buffer, 8);
			break;
		}

		//Loop until file name received
	}

	//Create file
	ofstream out(fileName, ios::trunc | ios::binary);

	//Receiving file loop
	while (length > 0)
	{
		ZeroMemory(buffer, BUFFER_SIZE);
		//Waiting for data
		int bytesReceived = Recv(fsender, buffer, BUFFER_SIZE, 0);
		if (bytesReceived == SOCKET_ERROR) return -2;

		if (length >= BUFFER_SIZE)
			out.write(buffer, BUFFER_SIZE);
		else
			out.write(buffer, length);
		length -= BUFFER_SIZE;
	}

	return 1;
}

/*
Sending file process
Precaution:
-    Both SendFileForServer and GetFile must start at the same time

Parameter:
-    freceiver: receiver's socket
-    dir: file directory (including file name)

Return:
-    1 = successful
-   -1 = connection error
-   -2 = cannot open file
*/
int Server::SendFileForServer(const SOCKET& freceiver, const string& dir)
{
	char buffer[BUFFER_SIZE];
	string userInput;
	int sendResult, bytesReceived;
	long long int length;

	//header 
	string filename = "0" + GetFileName(dir);

	//Send file name
	sendResult = Send_s(freceiver, filename, 0);
	if (sendResult == SOCKET_ERROR) return -1;

	//Open file
	ifstream file(dir, ifstream::binary);
	if (!file) return -2;

	//Get length of file:
	file.seekg(0, file.end);
	length = file.tellg();
	file.seekg(0, file.beg);

	//Send length
	memcpy(buffer + 1, &length, 8);
	//header 
	buffer[0] = '0';
	sendResult = Send(freceiver, buffer, 8 + 1, 0);
	if (sendResult == SOCKET_ERROR) return -1;
	//Send file
	while (length > 0)
	{
		ZeroMemory(buffer, BUFFER_SIZE);
		file.read(buffer + 1, BUFFER_SIZE - 1);
		//header 
		buffer[0] = '0';
		if (length >= BUFFER_SIZE - 1)
		{
			sendResult = Send(freceiver, buffer, BUFFER_SIZE, 0);
			length -= (BUFFER_SIZE - 1);
		}
		else
		{
			sendResult = Send(freceiver, buffer, length + 1, 0);
			length = 0;
		}
		
		if (sendResult == SOCKET_ERROR) return -1;
		//cout << length << "  ";
	}

	file.close();
	return 1;
}
#include "Client.h"

/*
	function execute client connect to server
	return false if the connection fail
*/
bool Client::Connect(string ipAddress) {
	if (clientSocket.Connect(ipAddress) == SOCKET_ERROR)
	{
		clientSocket.Disconnect();
		return false;
	}
	isConnected = true;

	bool outLoop = false;
	while (!outLoop) {
		switch (UI.drawFirstMenu(username, password)) {
		case 1:
			if (!SignIn()) {
				UI.drawSignNotification(1);
			}else
				outLoop = true;
			break;
		case 2:
			if (!SignUp()) {
				UI.drawSignNotification(2);
			}
			break;
		}
		if (!isConnected) {
			break;
		}
	}
	system("CLS");
	UI.resetActivity();
	UI.Initialize();
	return true;
}

/*
	function send Sign In request to server
*/
bool Client::SignIn() {
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
			Recv_NonNoti(signInRes,0);

			if (signInRes[0] == '1') {
				return true;		// Breakout the loop
			}
			else {
				return false;
			}
		}
		else {
			UI.drawNotification("Server isn't available !!! Try again later");
			isConnected = false;
			return false;
		}
	} while (1);
	return false;
}

/*
	function send Sign Up request to server
*/
bool Client::SignUp() {
	string notification = "SignUp";
	do {
		Send_s(clientSocket.GetSock(), notification, 0);

		//Append string
		int sendResult = Send_s(clientSocket.GetSock(), username, 0);		//Send sign up notification to Server
		sendResult = Send_s(clientSocket.GetSock(), password, 0);
		if (sendResult != SOCKET_ERROR) {
			
			string signUpRes;
			int size;
			Recv_NonNoti(signUpRes,0);
			
			if (signUpRes[0] == '1') {
				return true;
			}
			else {
				return false;
			}
		}
		else {
			UI.drawNotification("Server isn't available !!! Try again later");
			isConnected = false;
			return false;
		}
	} while (1);

}

/*
This function check every package and handle if it's a notification
*/
void Client::NotiHandle()
{
	char buffer[BUFFER_SIZE];
	int bytesReceived = 0;
	isNotiListenOn = true;
	while (bytesReceived != SOCKET_ERROR) 
	{
		//cout << "Noti wait\n";
		unique_lock<mutex> lck(mutex_);
		covaNoti.wait(lck, [this] { return (notiHandle); });
		//cout << "Noti awake\n";

		//Peek into every packet
		do
		{
			bytesReceived = recv(clientSocket.GetSock(), buffer, BUFFER_SIZE, MSG_PEEK);
		
			if (bytesReceived == SOCKET_ERROR)
			{
				//cout << "error here! with error code " << WSAGetLastError() << endl;
				//ServerShutdown();
				goto ErrorOccur;
			}
		} while (bytesReceived < (sizeof(int32_t) + 1));	//Only accept if peek enough bytes to check

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
		{
			//ServerShutdown();
			goto ErrorOccur;
		}
		lck.unlock();
		UI.drawNotification(buffer+1);
	}
	notiHandle = false;
	//ServerShutdown();
	isNotiListenOn = false;
	return;
ErrorOccur:
	notiHandle = false;
	isNotiListenOn = false;
	cout << "!!error!!";
	return;
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
	cin >> ws;
	getline(cin, fileDir);
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
	if (Send_s(clientSocket.GetSock(), notification, 0) == -1) {
		isConnected = false; 
		return -3; //server down
	}
	fileDir = InputFileToSend();

	//Get file dir & art file transfering
	int err = SendFile(clientSocket.GetSock() , fileDir);

	switch(1)
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
	string dir, filename;
	string fileDB, input;

	string notification = "Download";
	if (Send_s(clientSocket.GetSock(), notification, 0) == SOCKET_ERROR) {
		isConnected = false;
		return -3; //server down
	}

	//Get file database from server
	GetFile(fileDB, "");
	ifstream db(fileDB);

	//Get wanted files
	cout << "Choose file to download:\n"
		<< "Tip: you can enter multiple choices with each choice seperated\n"
		<< "from the orthers by a space.\n";

	string temp;
	int count = 0;
	while (getline(db, temp))
	{
		count++;
		cout << count << ". " << temp << "\n";
	}
	db.close();
	remove(fileDB.c_str());	//Remove db file in client after reading data success

	//DB have no file
	if (count == 0)
	{
		cout << "Server is empty, please upload any file first to be able to download.\n"
			<< "Press any key to go back\n";
		int sendResult = Send_s(clientSocket.GetSock(), "", 0);
		if (sendResult == SOCKET_ERROR) {
			isConnected = false;
			return -3; //server down
		}
		_getch();
		return 1;
	}
	cout << "> ";
	cin >> ws;
	getline(cin, input);

	//Get wanted diractory
	cout << "\nType in the directory you want your file to be saved in:\n"
		<< "Precaution: this apply to all the file you choose.\n"
		<< "> ";
	cin >> ws;
	getline(cin, dir);

	struct stat info;
	//validate directory
	while (1) {
		const char* d = dir.c_str();
		if (stat(d, &info) != 0) {
			cout << "Can not regconize your dir!!! Insert Again: ";
			cin >> ws;
			getline(cin, dir);
		}
		else {
			break;
		}
	}

	if (*dir.begin() == '\"')
		dir.erase(dir.begin());
	if (*(dir.end() - 1) == '\"')
		dir.erase(dir.end() - 1);

	//List all the wanted files and send to server
	stringstream userChoices(input);
	int i = 0, index;
	while (userChoices.good())
	{
		userChoices >> index;
		buffer[i++] = index;
	} 
	buffer[i] = '\0';

	int sendResult = Send(clientSocket.GetSock(), buffer, i + 1, 0);
	if (sendResult == SOCKET_ERROR) {
		isConnected = false;
		return -3; //server down
	}

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
	int bytesReceived = 0;

	//Get file name from sender

	ZeroMemory(buffer, BUFFER_SIZE);

	//Wait for server to send
	//Loop until file name received
		
	Recv_NonNoti(fileName, 0);
	if (!isConnected) {
		return -3;
	}

	//Get file size from client
	long long int length = 0;
	string data;

	ZeroMemory(buffer, BUFFER_SIZE);

	//Waiting for data
	Recv_NonNoti(buffer, bytesReceived, 0);
	if (!isConnected) {
		return -3;
	}
	//Checking data
	memcpy(&length, buffer, 8);
	
	//Checking directory
	string normDir = "";
	if (dir != "")
		if (*(dir.end() - 1) != '\\')
			normDir = dir + '\\';
		else
			normDir = dir;

	//Create file
	ofstream out(normDir + fileName, ios::trunc | ios::binary);

	//Receiving file loop
	while(length>0) {
		ZeroMemory(buffer, BUFFER_SIZE);
		//Waiting for data
		Recv_NonNoti(buffer, bytesReceived, 0);
		if (!isConnected) {
			return -3;
		}
		if (length >= (BUFFER_SIZE-1))
			out.write(buffer, (BUFFER_SIZE-1));
		else
			out.write(buffer, length);
		length -= (BUFFER_SIZE-1);

	}
	return 1;
}

/*
Extract file name from file directory
Parameter:
-    dir: file sirectory

Return:
-    file name
*/
string GetFileName(const string& dir)
{
	int length = dir.length();
	int pos = length - 1;
	while (dir[pos] != '\\' && pos != 0)
		pos--;
	if (dir[pos] == '\\') pos++;
	char res[255];
	length = dir.copy(res, length - pos, pos);
	res[length] = '\0';
	return string(res);
}

/*
Sending file process
Precaution:
-    Both SendFile and GetFile must start at the same time

Parameter:
-    freceiver: receiver's socket
-    dir: file directory (including file name)

Return:
-    1 = successful
-   -1 = connection error
-   -2 = cannot open file
*/
int Client::SendFile(const SOCKET& freceiver, const string& dir)
{
	char buffer[BUFFER_SIZE];
	string userInput;
	int sendResult;
	long long int length;

	//Send file name
	sendResult = Send_s(freceiver,GetFileName(dir), 0);
	if (sendResult == SOCKET_ERROR) {
		isConnected = false;
		return -3;
	}

	//Open file
	ifstream file(dir, ifstream::binary);
	if (!file) return -2;

	//Get length of file:
	file.seekg(0, file.end);
	length = file.tellg();
	file.seekg(0, file.beg);

	//Send length
	memcpy(buffer, &length, 8);
	sendResult = Send(freceiver, buffer, 8, 0);
	if (sendResult == SOCKET_ERROR) {
		isConnected = false;
		return -3;
	}
	//Send file
	while (length > 0)
	{
		ZeroMemory(buffer, BUFFER_SIZE);
		file.read(buffer, BUFFER_SIZE);
		if (length >= BUFFER_SIZE)
		{
			sendResult = Send(freceiver, buffer, BUFFER_SIZE, 0);
			length -= BUFFER_SIZE;
		}
		else
		{
			sendResult = Send(freceiver, buffer, length, 0);
			length = 0;
		}
		if (sendResult == SOCKET_ERROR) {
			isConnected = false;
			return -3;
		}
	}

	file.close();
	return 1;
}


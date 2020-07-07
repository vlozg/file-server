#include "Client.h"

/*
	Connect to server using the given address
	Return false if the connection failed

	(error can be retrived with GetLastError)
*/
bool Client::Connect(string ipAddress, int port)
{
	portServer = port;
	ipServer = ipAddress;

	clientSocket.Initialize(port);
	if (clientSocket.Connect(ipAddress) == SOCKET_ERROR)
	{
		//Must close socket in order to be able to call connect again
		clientSocket.Disconnect();
		SocketError();
		return false;
	}
	isConnected = true;
	return true;
}

/*
	Connect to server using the given address from last call of Connect
	Return false if the connection failed

	(error can be retrived with GetLastError)
*/
bool Client::ReConnect()
{
	clientSocket.Initialize(portServer);
	if (clientSocket.Connect(ipServer) == SOCKET_ERROR)
	{
		//Must close socket in order to be able to call connect again
		clientSocket.Disconnect();
		SocketError();
		return false;
	}
	isConnected = true;
	return true;
}

/*
	Disconnect from server in formal way
	(by user command, not by error occurrence)
*/
void Client::Disconnect()
{
	Send_s(clientSocket.GetSock(), "Disconnect", 0);
	isConnected = false;
	isNotiListenOn = false;
	clientSocket.Disconnect();
	TurnOffNotiHandle();
}

/*
	Send Sign In request to server
	Return false if sign in failed
*/
bool Client::SignIn(string& name, string& pass)
{
	//Send notification to Server
	Send_s(clientSocket.GetSock(), "SignIn", 0);

	//Send account information to server
	int sendResultUsername = Send_s(clientSocket.GetSock(), name, 0);
	int sendResultPassword = Send_s(clientSocket.GetSock(), pass, 0);

	if (sendResultUsername == SOCKET_ERROR
		|| sendResultPassword == SOCKET_ERROR)
	{
		SocketError();
		return false;
	}

	//Receive returned result from server
	string signInRes;
	Recv_NonNoti(signInRes, 0);

	if (signInRes[0] == '1')
	{
		username = name;
		return true;
	}
	else
		return false;
}

/*
	Send sign up request to server
	Client must sign in in order to
*/
bool Client::SignUp(string& name, string& pass)
{
	//Send flag
	Send_s(clientSocket.GetSock(), "SignUp", 0);

	//Append string
	int sendResult = Send_s(clientSocket.GetSock(), name, 0);
	sendResult = Send_s(clientSocket.GetSock(), pass, 0);

	if (sendResult == SOCKET_ERROR)
	{
		SocketError();
		return false;
	}

	//Receive returned result from server
	string signUpRes;
	Recv_NonNoti(signUpRes, 0);

	if (signUpRes[0] == '1')
		return true;
	else
		return false;
}

/*
	Sign out from server (does not delete socket)
*/
void Client::SignOut()
{
	Disconnect();
	ReConnect();
}

/*
Special Recv function that filter out every noti packet
*/
int Client::Recv_NonNoti(char* buffer, int32_t size, int flag) {
	char tbuffer[BUFFER_SIZE];
	int bytesReceived = 0;

	{
		//cout << "Recv wait\n";
		unique_lock<mutex> lck(mutex_);
		covaRecv.wait(lck, [this] { return !(notiHandle && isNotiListenOn); });
		//cout << "Recv awake\n";
		bytesReceived = Recv(clientSocket.GetSock(), tbuffer, BUFFER_SIZE, flag);
		notiHandle = true;
	}
	//cout << "R";
	covaNoti.notify_one();

	if (bytesReceived == SOCKET_ERROR)
	{
		SocketError();
		return SOCKET_ERROR;
	}

	//Remove header (post-processing)
	size = bytesReceived - 1;
	memcpy(buffer, tbuffer + 1, size);
	return size;
}

/*
Special Recv function that filter out every noti packet
*/
int Client::Recv_NonNoti(string& buffer, int flag) {
	int32_t size;
	char tbuffer[BUFFER_SIZE];
	int bytesReceived = 0;

	{
		//cout << "Recv wait\n";
		unique_lock<mutex> lck(mutex_);
		covaRecv.wait(lck, [this] { return !(notiHandle && isNotiListenOn); });
		//cout << "Recv awake\n";
		bytesReceived = Recv(clientSocket.GetSock(), tbuffer, BUFFER_SIZE, flag);
		notiHandle = true;
	}
	//cout << "R";
	covaNoti.notify_one();

	if (bytesReceived == SOCKET_ERROR)
	{
		SocketError();
		return SOCKET_ERROR;
	}

	//Remove header
	size = bytesReceived - 1;
	buffer = (tbuffer + 1);
	return size;
}

/* function handle Send File name to server and wait for
	server to send the file
	param:
		string filename: name of file
		string dir: directory to save
	Return:
-    1 = successful
-    -1 = connection error
-    -2 = error in the middle of file transfer process

*/
int Client::GetFileFromServer(string filename, string dir = "") 
{
	//Checking directory
	string normDir = "";
	if (dir != "")
		if (*(dir.end() - 1) != '\\')
			normDir = dir + '\\';
		else
			normDir = dir;

	string fullDir = dir + '/' + filename;

	//Send filename to server
	int sendRes = Send(clientSocket.GetSock(), fullDir.c_str(), fullDir.size(), 0);
	if (sendRes != 1) {
		return sendRes;
	}

	//Get the file from server
	string tmp;
	int res = GetFile(tmp, normDir);
	if (res < 0)
		return -1;
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
*/
int Client::GetFile(string& fileName, const string& dir)
{
	char buffer[BUFFER_SIZE];
	int bytesReceived = 0;

	//Get file name from sender
	Recv_NonNoti(fileName, 0);
	if (!isConnected) return -1;

	//Get file size from client
	long long int length = 0;

	Recv_NonNoti(buffer, bytesReceived, 0);
	if (!isConnected) return -1;
	memcpy(&length, buffer, 8);

	//Normalize directory
	string normDir = "";
	if (dir != "")
		if (*(dir.end() - 1) != '\\')
			normDir = dir + '\\';
		else
			normDir = dir;

	//Create file
	ofstream out(normDir + fileName, ios::trunc | ios::binary);

	//Receiving file loop
	while (length > 0)
	{
		//Waiting for data
		Recv_NonNoti(buffer, bytesReceived, 0);
		if (!isConnected) return -1;

		if (length >= (BUFFER_SIZE - 1))
			out.write(buffer, (BUFFER_SIZE - 1));
		else
			out.write(buffer, length);

		length -= (BUFFER_SIZE - 1);
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
-   -1 = error, call GetLastError function to get error detail
*/
int Client::SendFile(const SOCKET& freceiver, const string& dir)
{
	char buffer[BUFFER_SIZE];
	string userInput;
	int sendResult;
	long long int length;

	//Send file name
	sendResult = Send_s(freceiver, GetFileName(dir), 0);
	if (sendResult == SOCKET_ERROR) {
		SocketError();
		return -1;
	}

	//Open file
	ifstream file(dir, ifstream::binary);
	if (!file)
	{
		SetError("Cannot open file");
		return -1;
	}

	//Get length of file:
	file.seekg(0, file.end);
	length = file.tellg();
	file.seekg(0, file.beg);

	//Send length
	memcpy(buffer, &length, 8);
	sendResult = Send(freceiver, buffer, 8, 0);
	if (sendResult == SOCKET_ERROR) {
		SocketError();
		return -1;
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
			SocketError();
			return -1;
		}
	}

	file.close();
	return 1;
}


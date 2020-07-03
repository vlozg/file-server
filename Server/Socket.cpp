#include "Socket.h"

/*Alternative send function that check for multiple packets*/
int Send(SOCKET receiver, const char* buffer, int32_t size, int flag)
{
	int sendResult;
	//Include packet size in the head of packet
	sendResult = send(receiver, (char*)&size, sizeof(size), flag);
	if (sendResult == SOCKET_ERROR)
		return sendResult;

	sendResult = send(receiver, buffer, size, flag);
	if (sendResult == SOCKET_ERROR)
		return sendResult;

	return size;
}

/*
Alternative send function that check for multiple packets
This function is specialized for string due to the know of size
*/
int Send_s(SOCKET receiver, const string& buffer, int flag)
{
	int32_t size = buffer.length() + 1;
	int sendResult;
	//Include packet size in the head of packet
	sendResult = send(receiver, (char*)&size, sizeof(size), flag);
	if (sendResult == SOCKET_ERROR)
		return sendResult;

	sendResult = send(receiver, buffer.c_str(), size, flag);
	if (sendResult == SOCKET_ERROR)
		return sendResult;

	return size;
}

/*
Alternative recv function that check for multiple packets
Size is IGNORED
*/
int Recv(SOCKET sender, char* buffer, int32_t size, int flag)
{
	int bytesReceived = 0;	//Check for total bytes received to ensure receiving full packet
	ZeroMemory(buffer, BUFFER_SIZE);	//Clear buffer

	//Receive packet size first
	int receiveResult;
	do
	{
		//Must add the number of received bytes
		receiveResult = recv(sender, (char*)&size + bytesReceived, sizeof(size) - bytesReceived, flag);
		if (receiveResult == SOCKET_ERROR)
			return receiveResult;
		bytesReceived += receiveResult;	//Add up the number of received bytes
	} while (bytesReceived < sizeof(size));	//In case bytes reveived < bytes expected

	bytesReceived = 0;
	//Receive buffer based on size received
	do
	{
		//Must add the number of received bytes
		receiveResult = recv(sender, buffer + bytesReceived, size - bytesReceived, flag);
		if (receiveResult == SOCKET_ERROR)
			return receiveResult;
		bytesReceived += receiveResult;	//Add up the number of received bytes
	} while (bytesReceived < size);	//In case bytes reveived < bytes expected

	return bytesReceived;
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
int SendFile(const SOCKET& freceiver, const string& dir)
{
	char buffer[BUFFER_SIZE];
	string userInput;
	int sendResult, bytesReceived;
	long long int length;

	//Send file name
	sendResult = Send_s(freceiver, GetFileName(dir), 0);
	if (sendResult == SOCKET_ERROR) return -1;

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
	if (sendResult == SOCKET_ERROR) return -1;

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
		if (sendResult == SOCKET_ERROR) return -1;
	}

	file.close();
	return 1;
}
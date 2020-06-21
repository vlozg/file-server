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
	int32_t size = buffer.length()+1;
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
	receiveResult = recv(sender, (char*)&size, sizeof(size), flag);
	if (receiveResult == SOCKET_ERROR)
		return receiveResult;

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
#include "Server.h"




/* 
	function handle sign in request
*/
bool Server::SignInHandle(string name_pass) {
	fstream fin;
	fin.open("accounts.txt", ios::in);
	while (!fin.eof()) {
		string currUser;
		getline(fin, currUser);
		if (name_pass.compare(currUser) == 0) {
			fin.close();
			return true;
		}
	}
	fin.close();
	return false;
}

//Function register new account
bool Server::SignUpHandle(string username, string password) {
	fstream fout;
	fout.open("accounts.txt", ios::in);
	while (!fout.eof()) {
		string currusername, seek;
		getline(fout, currusername, '|');
		getline(fout, seek);					//move the pointer to next line
		if (currusername.compare(username) == 0) {
			fout.close();
			return false;
		}
	}
	fout.close();
	fout.open("accounts.txt", ios::app);
	fout << "\n" << username << "|" << password;
	fout.close();
	return true;
}

int Server::ReceiveFile(const SOCKET& client) {
	cout << "Start receiving file" << endl;
	char buffer[BUFFER_SIZE];
	string fileName;

	//Get file name from client
	while (1)
	{
		ZeroMemory(buffer, 4096);

		//Wait for client to send data
		int bytesReceived = recv(client, buffer, BUFFER_SIZE, 0);
		if (bytesReceived == SOCKET_ERROR) {
			cerr << "Error in recv(). Quitting !!!" << endl;
			return -1;
		}

		//Checking data
		if (bytesReceived > 1) {
			if (buffer[0] != 'f') continue;

			fileName = buffer + 1;
			break;
		}

		//Loop until file name received
	}

	//Get file size from client
	long long int length = 0;
	while (1)
	{
		ZeroMemory(buffer, 4096);

		//Wait for client to send data
		int bytesReceived = recv(client, buffer, BUFFER_SIZE, 0);
		if (bytesReceived == SOCKET_ERROR) {
			cerr << "Error in recv(). Quitting !!!" << endl;
			return -1;
		}

		//Checking data
		if (bytesReceived) {
			memcpy(&length, buffer, 8);
			break;
		}

		//Loop until file name received
	}

	//Check for duplicated file
		//Prompt to change the file name or overwrite

	//Create temporary file
	ofstream out(fileName, ios::trunc | ios::binary);

	//Receiving file loop
	while (length > 0)
	{
		ZeroMemory(buffer, 4096);
		int bytesReceived = recv(client, buffer, BUFFER_SIZE, 0);
		if (bytesReceived == SOCKET_ERROR) {
			cerr << "Error in recv(). Quitting !!!" << endl;
			return -1;
		}
		if (length >= BUFFER_SIZE)
			out.write(buffer, BUFFER_SIZE);
		else
			out.write(buffer, length);
		length -= BUFFER_SIZE;
	}

	//Send received success signal

	//Change temporary file name

	cout << "End receiving file";
	return 1;
}
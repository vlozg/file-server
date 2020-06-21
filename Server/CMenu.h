#pragma once
#include "Client.h"

class CMenu {
private:
	HANDLE window;
public:
	CMenu();
	bool checkValidName(string s);							//method to check  username and password valid

	void drawBorder(int width, int height);					//method draw Border
	
	void drawFirstMenu(Client client);									//method draw first menu
	
	bool drawSignInMenu(Client client);									//method draw sign in menu
	
	void drawSignUpMenu(Client client);									//method draw sign up menu
	
	string getPassword(Client client);									//method get password 

	string getUsername(Client client);									//method get username
};


#pragma once
#include "Socket.h"

using namespace std;
class CMenu {
private:
	HANDLE window;
	COORD cursorNotification;
	COORD cursorUser;
	COORD sysCursor;
public:
	CMenu();
	bool checkValidName(string s);							//method to check  username and password valid

	void drawBorder(int width, int height);					//method draw Border
	
	int drawFirstMenu(string& username, string& password);									//method draw first menu
	
	void drawSignInMenu(string& username, string& password);									//method draw sign in menu
	
	void drawSignUpMenu(string& username, string& password);									//method draw sign up menu

	void drawSignNotification(int type);					//method draw sign up and sign in result type = 1 sign in, type = 2 sign up
	
	string getPassword(string username, string password);									//method get password 

	string getUsername(string username, string password);									//method get username


	void drawNotification(string);							//method draw notification


	void resetActivity();									//method clear screen of activity area when user change action

	void Initialize();										//method initiazlize console window after sign in success
};


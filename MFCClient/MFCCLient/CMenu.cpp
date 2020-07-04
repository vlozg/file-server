#include "pch.h"
#include "CMenu.h"

CMenu1::CMenu1() {
	window = GetStdHandle(STD_OUTPUT_HANDLE);
	cursorUser = { 0,1 };
	sysCursor = { 0,0 };
	cursorNotification = { 0,0 };
}


bool CMenu1::checkValidName(string s) {
	//username can't be empty
	if (s.empty()) {
		return false;
	}

	int isNum = 0, isChar = 0;					// variable count numbers and charaters
	for (int i = 0; i < s.length(); i++) {
		if (s[i] >= 48 && s[i] <= 57) {			//0 -> 9
			++isNum;
		}
		else if((s[i] >= 65 && s[i] <= 90) || (s[i] >= 97 && s[i] <= 122)){		//a->z, A->Z
			++isChar;
		}
		else {
			return false;						// is special charater
		}
	}
	return (isNum > 0 && isChar > 0);			//username must be have both number and character
}


void CMenu1::drawBorder(int width, int height) {
	char HL = '\xCD'; // horizontal line
	char VL = '\xBA'; // vertical line
	char TL = '\xC9'; // top left edge
	char TR = '\xBB'; // top right edge
	char BL = '\xC8'; // bottom left edge
	char BR = '\xBC'; // bottom right edge

	//hande top line
	string top(width - 2, HL);
	top.insert(top.begin(), TL);
	top.push_back(TR);

	//handle bottom line
	string bottom(width - 2, HL);
	bottom.insert(bottom.begin(), BL);
	bottom.push_back(BR);

	//hadle left and right line
	string left, space(width - 2, ' ');
	for (int i = 0; i < height; i++) {
		left.push_back(VL);
		left.append(space);
		left.append("\xBA\n");
	}
	cout << top << endl;
	cout << left;
	cout << bottom << endl;

}


void CMenu1::drawSignInMenu(string& username, string& password) {
	
	//Initialize
	//if (username.empty()) {
	SetConsoleCursorPosition(window, { 0,0 });
	drawBorder(50, 6);
	//}

	COORD cursor = { 2,1 };
	SetConsoleCursorPosition(window, cursor);
	cout << "USERNAME:                                    ";
	cursor = { 2,3 };
	SetConsoleCursorPosition(window, cursor);
	cout << "PASSWORD:                                    ";
	cursor = { 2,5 };
	SetConsoleCursorPosition(window, { 2,8 });
	cout << "Press ESC to return!!";

	username.clear();
	password.clear();
	//Input
	//input username
	cursor = { 12,1 };
	SetConsoleCursorPosition(window, cursor);
	username = getUsername(username,password);
	if (username.empty()) {
		return;
	}

	//input password
	cursor = { 12,3 };
	SetConsoleCursorPosition(window, cursor);
	password = getPassword(username,password);
	if (password.empty()) {
		return;
	}

}


void CMenu1::drawSignUpMenu(string& username, string& password) {
	//initialize
	if (username.empty()) {
		SetConsoleCursorPosition(window, { 0,0 });
		drawBorder(80, 8);
	}
	COORD cursor = { 2,1 };
	SetConsoleCursorPosition(window, cursor);
	cout << "USERNAME:                                                          ";
	cursor = { 2,3 };
	SetConsoleCursorPosition(window, cursor);
	cout << "PASSWORD:                                                          ";
	cursor = { 2,5 };
	SetConsoleCursorPosition(window, cursor);
	cout << "CONFIRM PASSWORD:                                                  ";
	username.clear();
	password.clear();
	string rePassword;
	SetConsoleCursorPosition(window, {2,10});
	cout << "Press ESC to return!!";
	//input username
	while (1) {
		//input username
		cursor = { 12,1 };
		SetConsoleCursorPosition(window, cursor);
		username = getUsername(username,password);
		if (username.empty()) {
			return;
		}

		//remove notification
		cursor = { 2,7 };
		SetConsoleCursorPosition(window, cursor);
		cout << "                                                                             ";

		//check valide username
		if (checkValidName(username)) {
			break;
		}
		else {
			cursor = { 2,7 };
			SetConsoleCursorPosition(window, cursor);
			cout << "Username can't contain special characters, have number and character!";
			username.clear();

			//remove old username
			cursor = { 12, 1 };
			SetConsoleCursorPosition(window, cursor);
			cout << "                                                     ";
		}
	}

	//input password
	while (1) {
		//input password
		cursor = { 12,3 };
		SetConsoleCursorPosition(window, cursor);
		password = getPassword(username,password);
		if (password.empty()) {
			return;
		}
		//remove notification
		cursor = { 2,7 };
		SetConsoleCursorPosition(window, cursor);
		cout << "                                                                             ";

		//check valid password
		if (checkValidName(password)) {
			break;
		}
		else {
			cursor = { 2,7 };
			SetConsoleCursorPosition(window, cursor);
			cout << "Password can't contain special characters, have number and character!";
			password.clear();

			//remove old username
			cursor = { 12,3 };
			SetConsoleCursorPosition(window, cursor);
			cout << "                                              ";
		}
	}

	//space confirm password
	cursor = { 20,5 };
	SetConsoleCursorPosition(window, cursor);
	rePassword = getPassword(username, rePassword);
	if (rePassword.empty()) {
		return;
	}

	//check password and confirm pass word
	while (password.compare(rePassword) != 0) {
		password.clear();
		rePassword.clear();
		cursor = { 2,7 };
		SetConsoleCursorPosition(window, cursor);
		cout << "Those password didn't match! Try again!";
		
		//Remove old data
		//remove password
		cursor = { 12,3 };
		SetConsoleCursorPosition(window, cursor);
		cout << "                                              ";
		//remove confirm password
		cursor = { 20,5 };
		SetConsoleCursorPosition(window, cursor);
		cout << "                                      ";
			

		//input password
		cursor = { 12,3 };
		SetConsoleCursorPosition(window, cursor);
		password = getPassword(username,password);
		if (password.empty()) {
			return;
		}
			
		//remove notification
		cursor = { 2,7 };
		SetConsoleCursorPosition(window, cursor);
		cout << "                                        ";				

		//input confirm password
		cursor = { 20,5 };
		SetConsoleCursorPosition(window, cursor);
		rePassword = getPassword(username, rePassword);
		if (rePassword.empty()) {
			password.clear();
			return;
		}
	}
}

void CMenu1::drawSignNotification(int type) {
	if (type == 1) {
		SetConsoleCursorPosition(window, { 2,5 });
		cout << "Username or password is incorrect! Try again!!";
	}
	if (type == 2) {
		SetConsoleCursorPosition(window, { 2,7 });
		cout << "Sign up failed! This username has taken. Try another";
	}
	Sleep(1000);
}


int CMenu1::drawFirstMenu(string &username,string &password) {
	while (1) {
		username.clear();
		password.clear();
		system("CLS");
		drawBorder(80, 6);
		string sel;
		COORD cursor = { 2,1 };
		SetConsoleCursorPosition(window, cursor);
		cout << "Input 1: Sign In           --------            Input 2: Sign Up \n\n";
		cursor = { 2,3 };
		SetConsoleCursorPosition(window, cursor);
		cout << "Your selection: ";
		cin >> sel;
		if (sel == "1") {
			system("CLS");
			drawSignInMenu(username, password);
			if (username.empty() || password.empty()) {
				continue;
			}

			return 1;
		}
		else if (sel == "2") {
			system("CLS");
			drawSignUpMenu(username, password);
			if (username.empty() || password.empty()) {
				continue;
			}
			return 2;
		}
		else {
			cout << "\n Invalid selection!!";
			cursor = { 2,3 };
			SetConsoleCursorPosition(window, cursor);
			cout <<"                                       ";
		}
	}
}



string CMenu1::getPassword(string username, string password) {
	char c = 0;
	do {			// 13 in ASCII is Enter
		c = _getch();
		if (c == 8) {			// 8 in ASCII is Backspace
			if (!password.empty()) {
				password.pop_back();
				cout << "\b \b";
			}
		}
		else if (c == 13) {
			break;
		}
		else if (c == 27) {					//27 = ESC return to first menu
			//drawFirstMenu(username,password);
			return "";
		}
		else {
			password.push_back(c);
			cout << "* \b";
		}
	} while (1);
	cout << endl;
	return password;
}

string CMenu1::getUsername(string username, string password) {
	char c = 0;
	do {			// 13 in ASCII is Enter
		c = _getch();
		if (c == 8) {			// 8 in ASCII is Backspace
			if (!username.empty()) {
				username.pop_back();
				cout << "\b \b";
			}
		}
		else if (c == 13) {
			break;
		}
		else if (c == 27) {
		//	drawFirstMenu(username,password);
			return "";
		}
		else {
			username.push_back(c);
			cout << c <<" \b";
		}
	} while (1);
	cout << endl;
	return username;
}


void CMenu1::drawNotification(string newNotification) {
	cursorNotification.Y += 1;
	COORD temp;
	CONSOLE_SCREEN_BUFFER_INFO cbsi;
	GetConsoleScreenBufferInfo(window, &cbsi);
	temp = cbsi.dwCursorPosition;
	SetConsoleCursorPosition(window, cursorNotification);
	cout << newNotification;
	SetConsoleCursorPosition(window, temp);
}



void CMenu1::resetActivity() {
	for (SHORT i = sysCursor.Y - 1; i > 0; i--) {
		SetConsoleCursorPosition(window, { 0,i });
		cout << "                                                                               ";
	}
	SetConsoleCursorPosition(window, { 0,1 });				//move cursor to bottom of windows
}

void CMenu1::Initialize() {
	SetConsoleCursorPosition(window, { 0,0 });
	cout << "                                    Activity                                   \xBA";
	cursorNotification = { 80,0 };
	SetConsoleCursorPosition(window, cursorNotification);
	cout << "            Notifications             ";
	cursorNotification.Y++;

	//draw line
	COORD temp = { 79,1 };
	SetConsoleCursorPosition(window, temp);
	for (int i = 0; i < 100; i++) {
		cout << '\xBA';
		temp.Y += 1;
		SetConsoleCursorPosition(window, temp);
	}
	temp = { 0,101 };
	sysCursor = temp;
	SetConsoleCursorPosition(window, cursorUser);
}
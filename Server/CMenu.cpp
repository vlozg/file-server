#include "CMenu.h"


CMenu::CMenu() {
	window = GetStdHandle(STD_OUTPUT_HANDLE);
}


bool CMenu::checkValidName(string s) {
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


void CMenu::drawBorder(int width, int height) {
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


bool CMenu::drawSignInMenu(Client client) {
	
	//Initialize
	drawBorder(50, 6);

	COORD cursor = { 2,1 };
	SetConsoleCursorPosition(window, cursor);
	cout << "USERNAME:";
	cursor = { 2,3 };
	SetConsoleCursorPosition(window, cursor);
	cout << "PASSWORD: ";
	cursor = { 2,5 };
	SetConsoleCursorPosition(window, cursor);
	cout << "                                             ";

	//Input
	while (1) {
		
		string username, password;

		//inout username
		cursor = { 12,1 };
		SetConsoleCursorPosition(window, cursor);
		username = getUsername(client);

		//input password
		cursor = { 12,3 };
		SetConsoleCursorPosition(window, cursor);
		password = getPassword(client);

		//Handle Sign In here

		int res = client.SignIn(username, password);
		if (res) {
			return res;
		}

		//remove old data when sign in fail
		cursor = { 12,1 };
		SetConsoleCursorPosition(window, cursor);
		cout << "                                    ";
		cursor = { 12,3 };
		SetConsoleCursorPosition(window, cursor);
		cout << "                                    ";
		cursor = { 2,5 };
		SetConsoleCursorPosition(window, cursor);
		cout << "Username or Password is incorrect!";
	}
	return false;
}


void CMenu::drawSignUpMenu(Client client) {
	//initialize
	drawBorder(60, 8);
	COORD cursor = { 2,1 };
	SetConsoleCursorPosition(window, cursor);
	cout << "USERNAME:";
	cursor = { 2,3 };
	SetConsoleCursorPosition(window, cursor);
	cout << "PASSWORD: ";
	cursor = { 2,5 };
	SetConsoleCursorPosition(window, cursor);
	cout << "CONFIRM PASSWORD: ";
	while (1) {
		string username, password, rePassword;
		
		//input username
		while (1) {
			//input username
			cursor = { 12,1 };
			SetConsoleCursorPosition(window, cursor);
			username = getUsername(client);

			//remove notification
			cursor = { 2,7 };
			SetConsoleCursorPosition(window, cursor);
			cout << "                                                         ";

			//check valide username
			if (checkValidName(username)) {
				break;
			}
			else {
				cursor = { 2,7 };
				SetConsoleCursorPosition(window, cursor);
				cout << "Username can't contain special characters!";

				//remove old username
				cursor = { 12, 1 };
				SetConsoleCursorPosition(window, cursor);
				cout << "                                              ";
			}
		}

		//input password
		while (1) {
			//input password
			cursor = { 12,3 };
			SetConsoleCursorPosition(window, cursor);
			password = getPassword(client);

			//remove notification
			cursor = { 2,7 };
			SetConsoleCursorPosition(window, cursor);
			cout << "                                                         ";

			//check valid password
			if (checkValidName(password)) {
				break;
			}
			else {
				cursor = { 2,7 };
				SetConsoleCursorPosition(window, cursor);
				cout << "Password can't contain special characters!";

				//remove old username
				cursor = { 12,3 };
				SetConsoleCursorPosition(window, cursor);
				cout << "                                              ";
			}
		}

		//space confirm password
		cursor = { 20,5 };
		SetConsoleCursorPosition(window, cursor);
		rePassword = getPassword(client);

		//check password and confirm pass word
		while (password.compare(rePassword) != 0) {
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
			password = getPassword(client);
			
			//remove notification
			cursor = { 2,7 };
			SetConsoleCursorPosition(window, cursor);
			cout << "                                        ";				

			//input confirm password
			cursor = { 20,5 };
			SetConsoleCursorPosition(window, cursor);
			rePassword = getPassword(client);
		}
		
		// Handle Sign Up

		if (client.SignUp(username, password)) {
			return;
		}


		//notify if sign up fail
		cursor = { 2,7 };
		SetConsoleCursorPosition(window, cursor);
		cout << "Sign up failed! This username has taken. Try another!";
		
		//remove old data if sign up fail
		//remove confirm password
		cursor = { 20,5 };
		SetConsoleCursorPosition(window, cursor);
		cout << "                                      ";

		//remove password
		cursor = { 12,3 };
		SetConsoleCursorPosition(window, cursor);
		cout << "                                              ";

		//remove old username
		cursor = { 12,1 };
		SetConsoleCursorPosition(window, cursor);
		cout << "                                              ";
	}
}


void CMenu::drawFirstMenu(Client client) {
	system("CLS");
	drawBorder(80, 6);
	int sel;
	COORD cursor = { 2,1 };
	SetConsoleCursorPosition(window, cursor);
	cout << "Input 1: Sign In           --------            Input 2: Sign Up \n\n";
	while (1) {
		cursor = { 2,3 };
		SetConsoleCursorPosition(window, cursor);
		cout << "Your selection: ";
		cin >> sel;
		if (sel == 1) {
			system("CLS");
			if (drawSignInMenu(client)) {
				system("CLS");
				return;
			}
		}
		else if (sel == 2) {
			system("CLS");
			drawSignUpMenu(client);
			system("CLS");
			return;
		}
		else {
			cout << "\n Invalid selection!!";
			cursor = { 2,3 };
			SetConsoleCursorPosition(window, cursor);
			cout <<"                                       ";
		}
	}
}



string CMenu::getPassword(Client client) {
	string password;
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
			drawFirstMenu(client);
		}
		else {
			password.push_back(c);
			cout << "* \b";
		}
	} while (1);
	cout << endl;
	return password;
}

string CMenu::getUsername(Client client) {
	string username;
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
			drawFirstMenu(client);
		}
		else {
			username.push_back(c);
			cout << c <<" \b";
		}
	} while (1);
	cout << endl;
	return username;
}
#include "CMainWindow.h"

//Intructor method
CMainWindow::CMainWindow() {
	//Initialize position
	CursorListOnlineUsers = { 0,0 };
	CursorNotification = { 40,0 };
	CursorLog = { 80,0 };
	sysCursor = { 0, 4 };

	mainWindow = GetStdHandle(STD_OUTPUT_HANDLE);			//Initialize handle to console

	//draw line

	

	//Print label follow position
	cout << "           List Online Users          \xBA";
	SetConsoleCursorPosition(mainWindow, CursorNotification);
	cout << "             Notifications            \xBA";
	SetConsoleCursorPosition(mainWindow, CursorLog);
	cout << "              Users's Log               ";
}



void CMainWindow::drawListOnlineUsers(string newUser) {
	CursorListOnlineUsers.Y += 1;									//change position to new line
	SetConsoleCursorPosition(mainWindow, CursorListOnlineUsers);	//Move cursor to new position
	cout << newUser;
	drawLine(CursorListOnlineUsers.Y);
	setConsoleCursor();												//ensure notification of system can't overide data
}


void CMainWindow::drawNotification(string newNotification) {
	CursorNotification.Y += 1;
	SetConsoleCursorPosition(mainWindow, CursorNotification);
	cout << newNotification;
	drawLine(CursorNotification.Y);
	setConsoleCursor();
}

void CMainWindow::drawLog(string newEvent) {
	CursorLog.Y += 1;
	SetConsoleCursorPosition(mainWindow, CursorLog);
	cout << newEvent;
	drawLine(CursorLog.Y);
	setConsoleCursor();
}

void CMainWindow::setConsoleCursor() {
	
	//find max position of line 
	SHORT maxY = max(CursorListOnlineUsers.Y, CursorNotification.Y);
	maxY = max(maxY, CursorLog.Y);
	maxY = max(maxY + 1, sysCursor.Y);
	sysCursor = { 0,maxY };

	//move cursor to new position
	SetConsoleCursorPosition(mainWindow, sysCursor);
}

//Method use to clear data of list User when a user Sign out to refresh list of online users
void CMainWindow::resetCursorListOnlineUser() {

	//Reset data
	int currY = CursorListOnlineUsers.Y;
	for (SHORT i = currY; i > 0; i -= 1) {
		COORD currCoord = { 0,i };
		SetConsoleCursorPosition(mainWindow, currCoord);
		cout << "                                      ";					//Restore empty space to rewrite data
		CursorListOnlineUsers.Y -= 1;
	}
	setConsoleCursor();
}

void CMainWindow::drawLine(SHORT y) {
	COORD temp = { 38,y };
	SetConsoleCursorPosition(mainWindow, temp);
	cout << "\xBA";
	temp.X = 78;
	SetConsoleCursorPosition(mainWindow, temp);
	cout << "\xBA";
	temp.X = 119;
}
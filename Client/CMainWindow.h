#pragma once

#include <iostream>
#include <string>
#include "Windows.h"

using namespace std;

//
class CMainWindow {
private:
	COORD CursorListOnlineUsers;			//position of cursor to print list online users
	COORD CursorNotification;			//position of cursor to print Notification
	COORD CursorLog;					// position of cursor to print user's log
	HANDLE mainWindow;
	COORD sysCursor;					//cursor of system

public:
	CMainWindow();					
		
	void drawListOnlineUsers(string);			//method draw list online users
		
	void drawNotification(string);					// method draw notification

	void drawLog(string);								// method draw user's log

	void setConsoleCursor();						//method to ensure data not lost

	void resetCursorListOnlineUser();				//Reset position of this cursor when a user sign out
													//list user can be store in an array
	void drawLine(SHORT y);
};													




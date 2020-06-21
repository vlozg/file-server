#ifndef _FileTransfer_H_
#define _FileTransfer_H_

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#pragma once

#include <iostream>
#include <winsock2.h>
#include <stdio.h>
#include <tchar.h>
#include <WS2tcpip.h>
#include <sstream>
#include <fstream>

#define BUFFER_SIZE 4096

using namespace std;

int GetFile(const SOCKET& fsender, string& fileName, const string& dir = "");
int SendFile(const SOCKET& freceiver, const string& dir);

#endif
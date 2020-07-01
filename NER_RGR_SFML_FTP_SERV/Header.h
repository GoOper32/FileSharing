#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>

#include <iostream>
//#include <Winsock2.h>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <string>
#include <list>
#include <thread>
#include <vector>
#include <fstream>
#include <algorithm>
#include <bitset>
#include <shlwapi.h>
#include <sys/stat.h>


using namespace sf;

#pragma warning(disable: 4996)
//#pragma comment(lib, "ws2_32.lib")
#define SERVICE_PORT 21

using namespace std;

struct PATH {
	string full_path;
	bool isDirectory = false;
	string path;
	string linux_rights;
	int sizeB=0;
	string time;
};


struct client {
	TcpSocket sock;
	IpAddress ip;
	client() {
	}
};


struct USER_S {
	string login;
	string password;
	USER_S(string log, string pass) {
		login = log;
		password = pass;
	}
};



int convertTo_Int(string str);

void ParsMessage(char*, int, vector<string>&);

inline bool PATHIsDirectory(const std::wstring& path);

std::wstring s2ws(const std::string& s);

string PATH_EXE_PROGRAMM();

list<PATH> LIST_S(string relative_path);

int getFileSize(const char* fileName);

string cutData(string msg, bool time = true);

string data_izmemeniya_f(const char* file, bool time = true);

void PrintConnection(RenderWindow& window, unsigned startPos);


void ConvertToUnixPATH(string& path);

void ConvertToWindowsPath(string& path);

bool isRootDirectory(string path);

void deleteEndSlash(string& str);

void deleteBeginSlash(string& str);

void addEndSlach(string& str);

string getPreviousPath(string str);

void addBeginSlash(string& str);
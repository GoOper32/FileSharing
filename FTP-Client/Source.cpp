#include <iostream>
#include <Winsock2.h>
#include <Windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <string>
#include <thread>
#include <vector>
#include <fstream>
#include <list>
#include <ctime>


#pragma warning(disable: 4996)
#pragma comment(lib, "ws2_32.lib")
#define FTP_PORT 21
#define CMD_USER "USER"
#define CMD_PASS "PASS"
#define CMD_PASV "PASV"
#define CMD_LIST "LIST"
#define CMD_NLST "NLST"
#define CMD_TYPE_A "TYPE A"
#define CMD_TYPE_I "TYPE I"
#define CMD_RETR "RETR"
#define CMD_STOR "STOR"
#define CMD_CWD "CWD"


using namespace std;

list<string> statusMSG;

string LAST_CODE = "0";
string msg;
int count_change_msg = 0;
bool LOCK_MSG = false;
string currentPATH = "/";


int convertTo_Int(string str);

void print_data_server(SOCKET s) {
	char buffer[512];
	ZeroMemory(buffer, 512);
	while (true) {
		recv(s, (char*)&buffer, 512, 0);
		while (LOCK_MSG) {
		}
		LOCK_MSG = true;
		msg = buffer;
		LOCK_MSG = false;
		count_change_msg = 1;
		statusMSG.push_back(buffer);
		ZeroMemory(buffer, 512);
	}
}

void connection_PASV(SOCKET s) {
	char buffer[4096];
	ZeroMemory(buffer, 4096);
	while (true) {
		recv(s, (char*)&buffer, 4096, 0);
		cout << buffer << endl;
		break;
	}
}


int CODE_from_msg() {
	
	while (LOCK_MSG) {
		
	}
	LOCK_MSG = true;
	int counBRBN = 0;
	int firstSymbolCode_pr = 0;
	int firstSymbolCode = 0;
	for (int i = 0; i < msg.size() && msg[i] != 0;i++) {
		if (msg[i] == '\r' && msg[i + 1] == '\n') {
			counBRBN++;
			firstSymbolCode_pr = firstSymbolCode;
			firstSymbolCode = i + 2;
			i++;
		}
	} 
	count_change_msg = 0;
	
	
	string ans = "0";
	if (msg.size()) {
		ans = msg[firstSymbolCode_pr];
		ans += msg[firstSymbolCode_pr + 1];
		ans += msg[firstSymbolCode_pr + 2];
	}
	LOCK_MSG = false;
	return atoi(ans.c_str());
}

int searchPortFromMSG() {
	int i = 0;
	int first_1 = 0; int last_1 = 0;
	int first_2 = 0; int last_2 = 0;
	for (int i = 0; i < msg.size(); i++) {
		if (msg[i] == '(') {
			int tmp = 4;
			while (tmp) {
				if (msg[i] == ',') {
					tmp--;
				}
				i++;
			}
			first_1 = i;
			while (true) {
				if (msg[i] == ',') {
					break;
				}
				i++;
			}
			last_1 = i - 1;
			first_2 = ++i;
			while (true) {
				if (msg[i] == ')') {
					break;
				}
				i++;
			}
			last_2 = i - 1;

			break;
		}
	}
	string number_1;
	string number_2;
	do  {
		number_1 += msg[first_1];
			first_1++;
	} while (first_1 <= last_1);
	do {
		number_2 += msg[first_2];
		first_2++;
	} while (first_2 <= last_2);

	return atoi(number_1.c_str()) * 256 + atoi(number_2.c_str());
}

void EnterLogPass(string& login, string& password) {
	cout << "Enter LOGIN: ";
	cin >> login;
	cout << "Enter PASSWORD: ";
	cin >> password;
	return;
}

int main() {
	srand(time(0));
	string str_addr;
	cout << "Enter addr server: ";
	cin >> str_addr;
	string password;
	string login;
	EnterLogPass(login, password);


	WSADATA ws;
	WSAStartup(MAKEWORD(1, 1), &ws);

	SOCKET s; 
	if ((s = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		fprintf(stderr, "Can't create socket\n");
		exit(1);
	}
	
	sockaddr_in _addr;
	char buff[1024];

	
	ZeroMemory(&_addr, sizeof(_addr));								

	_addr.sin_family = AF_INET;										
	_addr.sin_addr.S_un.S_addr = inet_addr(str_addr.c_str());		
	_addr.sin_port = htons(u_short(FTP_PORT));						

	
	vector<thread> th;
	connect(s, (sockaddr*)&_addr, sizeof(_addr));

	th.push_back(thread(print_data_server, s));

	string str;
	str = CMD_USER;  str += ' '; str += login; str+="\r\n";
	send(s, str.c_str(), str.size(), 0);
	str = CMD_PASS;  str += ' '; str += password; str += "\r\n";
	send(s, str.c_str(), str.size(), 0);
	Sleep(1000);
	string chooice;
	bool authorisation = false;
	while (true) {

		if (count_change_msg > 0) {
			Sleep(500);
			if (CODE_from_msg() == 230) { //��������
				cout << endl;
				cout << "#*#*#*#*#*#*#*#*#*#*#*#*#*#" << endl;
				cout << "#*SUCESSFUL AUTHORISATION*#" << endl;
				cout << "#*#*#*#*#*#*#*#*#*#*#*#*#*#" << endl << endl;

				break;
			}
		}
		
	}
	while (true) {
		

		cout << "> Select action" << endl; 
		cout << "\t\t[1] - Get an Extended list of files" << endl;
		cout << "\t\t[2] - Provide a list of files" << endl;
		cout << "\t\t[3] - Change Directory" << endl;
		cout << "\t\t[4] - Uploading a file from the server" << endl;
		cout << "\t\t[5] - Upload a file to the server" << endl;
		cout << "\t\t[6] - Disconnect" << endl;
		cout << "> You choice: "; cin >> chooice;

		if (chooice[0] == '1') {
			SOCKET c; c = socket(AF_INET, SOCK_STREAM, 0);
			sockaddr_in _addrPASV;
			ZeroMemory(&_addrPASV, sizeof(_addrPASV));
			_addrPASV.sin_family = AF_INET;
			_addrPASV.sin_addr.S_un.S_addr = inet_addr(str_addr.c_str());

			str = CMD_TYPE_A; str += "\r\n";
			send(s, str.c_str(), str.size(), 0);
			str = CMD_PASV; str += "\r\n";
			send(s, str.c_str(), str.size(), 0);
			Sleep(500);
			if (count_change_msg > 0) {
				if (CODE_from_msg() == 227) {
					_addrPASV.sin_port = htons(u_short(searchPortFromMSG()));
					connect(c, (sockaddr*)&_addrPASV, sizeof(_addrPASV));

					th.push_back(thread(connection_PASV, c));
					cout << endl << endl;
					str = CMD_LIST; str += "\r\n";
					send(s, str.c_str(), str.size(), 0);
					Sleep(500);
				}
			}
			closesocket(c);
		}
		else if (chooice[0] == '2') {
			SOCKET c; c = socket(AF_INET, SOCK_STREAM, 0);
			sockaddr_in _addrPASV;
			ZeroMemory(&_addrPASV, sizeof(_addrPASV));
			_addrPASV.sin_family = AF_INET;
			_addrPASV.sin_addr.S_un.S_addr = inet_addr(str_addr.c_str());

			str = CMD_TYPE_A; str += "\r\n";
			send(s, str.c_str(), str.size(), 0);
			str = CMD_PASV; str += "\r\n";
			send(s, str.c_str(), str.size(), 0);
			Sleep(500);
			if (count_change_msg > 0) {
				if (CODE_from_msg() == 227) {
					_addrPASV.sin_port = htons(u_short(searchPortFromMSG()));
					connect(c, (sockaddr*)&_addrPASV, sizeof(_addrPASV));

					th.push_back(thread(connection_PASV, c));
					str = CMD_NLST; str += "\r\n";
					cout << endl << endl;
					send(s, str.c_str(), str.size(), 0);
					Sleep(500);
				}
			}
			closesocket(c);
		}
		else if (chooice[0] == '3') {

			string pathDIR;
			cout << "Enter path "; cin >> pathDIR;
			if (pathDIR[0] == '/')
				pathDIR.erase(pathDIR.begin());
			pathDIR = currentPATH + pathDIR;
			if (pathDIR[pathDIR.size() - 1] != '/')
				pathDIR += '/';
			str = CMD_CWD; str += ' ' + pathDIR; str += "\r\n";
			send(s, str.c_str(), str.size(), 0);
			Sleep(500);
				if (CODE_from_msg() == 250) {
					currentPATH = pathDIR;
					Sleep(200);
					cout << "Current Directory: "<<currentPATH<<endl;
				}
		}
		else if (chooice[0] == '4') {
			string nameFile;
			cout << "Enter the file name: "; cin >> nameFile;
			if (nameFile[0] == '/')
				nameFile.erase(nameFile.begin());

			SOCKET c; c = socket(AF_INET, SOCK_STREAM, 0);
			sockaddr_in _addrPASV;
			ZeroMemory(&_addrPASV, sizeof(_addrPASV));
			_addrPASV.sin_family = AF_INET;
			_addrPASV.sin_addr.S_un.S_addr = inet_addr(str_addr.c_str());

			str = CMD_TYPE_A; str += "\r\n";
			send(s, str.c_str(), str.size(), 0);
			str = CMD_PASV; str += "\r\n";
			send(s, str.c_str(), str.size(), 0);
			Sleep(500);
			if (count_change_msg > 0) {
				if (CODE_from_msg() == 227) {
					_addrPASV.sin_port = htons(u_short(searchPortFromMSG()));
					connect(c, (sockaddr*)&_addrPASV, sizeof(_addrPASV));

					th.push_back(thread(connection_PASV, c));
					str = CMD_RETR; str += ' '; str += nameFile; str += "\r\n";
					cout << endl << endl;
					send(s, str.c_str(), str.size(), 0);
					Sleep(500);
				}
			}
			closesocket(c);
		}
		else if (chooice[0] == '5') {
			string tmpDATA;
			string nameFile;
			string dataFile;

			cout << "Enter Name file: "; cin >> nameFile;
			
			ifstream fin(nameFile);
			bool first_space = false;
			if (fin.is_open()) {
				while (fin >> tmpDATA) {
					if (first_space)
						dataFile += ' ';
					dataFile+=tmpDATA;
					first_space = true;
				}

				SOCKET c; c = socket(AF_INET, SOCK_STREAM, 0);
				sockaddr_in _addrPASV;
				ZeroMemory(&_addrPASV, sizeof(_addrPASV));
				_addrPASV.sin_family = AF_INET;
				_addrPASV.sin_addr.S_un.S_addr = inet_addr(str_addr.c_str());



				str = CMD_TYPE_I; str += "\r\n";
				send(s, str.c_str(), str.size(), 0);
				str = CMD_PASV; str += "\r\n";
				send(s, str.c_str(), str.size(), 0);
				Sleep(500);
				if (count_change_msg > 0) {
					if (CODE_from_msg() == 227) {
						_addrPASV.sin_port = htons(u_short(searchPortFromMSG()));
						connect(c, (sockaddr*)&_addrPASV, sizeof(_addrPASV));

						th.push_back(thread(connection_PASV, c));
						str = CMD_STOR; str += ' '; str += nameFile; str += "\r\n";
						cout << endl << endl;
						send(s, str.c_str(), str.size(), 0);
						str = dataFile;
						send(c, str.c_str(), str.size(), 0);
						Sleep(500);
					}
				}
				closesocket(c);
			}
			else {
				cout << "FILE NOT FOUND" << endl;
			}
		}
		else if(chooice[0] == '5'){
			break;
		}
		else {
			cout << "The command was not recognized" << endl;
		}
		cout << endl;
		if (statusMSG.size() >= 5) {
			string tmpDATA;
			string nameFile;
			string dataFile;

			for (auto it = statusMSG.begin(); it != statusMSG.end(); it++) {
				dataFile += *it;
			}
			statusMSG.clear();
			char buffer[15];
			time_t seconds = time(NULL);
			tm* timeinfo = localtime(&seconds);
			itoa(timeinfo->tm_year  , buffer, 10);	nameFile += buffer;
			itoa(timeinfo->tm_mday, buffer, 10);	nameFile += buffer;
			itoa(timeinfo->tm_hour, buffer, 10);	nameFile += buffer;
			itoa(timeinfo->tm_min, buffer, 10);	nameFile += buffer;
			itoa(timeinfo->tm_sec, buffer, 10);	nameFile += buffer;
			itoa(rand()%10000, buffer, 10);	nameFile += buffer;
			nameFile += ".log";

			


			SOCKET c; c = socket(AF_INET, SOCK_STREAM, 0);
			sockaddr_in _addrPASV;
			ZeroMemory(&_addrPASV, sizeof(_addrPASV));
			_addrPASV.sin_family = AF_INET;
			_addrPASV.sin_addr.S_un.S_addr = inet_addr(str_addr.c_str());



			str = CMD_TYPE_I; str += "\r\n";
			send(s, str.c_str(), str.size(), 0);
			str = CMD_PASV; str += "\r\n";
			send(s, str.c_str(), str.size(), 0);
			Sleep(500);
			if (count_change_msg > 0) {
				if (CODE_from_msg() == 227) {
					_addrPASV.sin_port = htons(u_short(searchPortFromMSG()));
					connect(c, (sockaddr*)&_addrPASV, sizeof(_addrPASV));

					th.push_back(thread(connection_PASV, c));
					str = CMD_STOR; str += ' '; str += nameFile; str += "\r\n";
					send(s, str.c_str(), str.size(), 0);
					str = dataFile;
					send(c, str.c_str(), str.size(), 0);
					Sleep(500);
				}
			}
			closesocket(c);
		}
	}

		

	return 0;
}

int convertTo_Int(string str) {
	int answer = 0;
	int multiplay = 10;
	if (str.size())
		answer = str[0] - '0';
	for (int i = 1; i < str.size(); i++) {
		answer *= 10;
		answer += str[i] - '0';
	}
	return answer;
}
#pragma once
#include "Header.h"

TcpSocket clientDataChanel[256];
bool dataChanelOpen_g[256] = { false };

class FTPServ
{
private:
	bool anonymous[256] = { false };
	bool user[256] = { false };
	bool dataChanel[256] = { false };
	bool *dataChanelOpen = dataChanelOpen_g;
	bool binaryMode[256] = { false };

public:
	bool authorisation[256] = { false };


	TcpListener listener;
	TcpSocket ServSock;
	list<USER_S> USERS;



	string userName[256];

	vector<string> MSG;
	string CurrentDirrectory[256];

	TcpSocket client[256];
	TcpSocket* clientDataChanels = clientDataChanel;

	list<PATH> list_files;
	vector<string> connectionUser;

	FTPServ() {
		USERS.push_back(USER_S(string("Ivan01"), string("IvanPASS")));
		//IpAddress ServIP = sf::IpAddress::getLocalAddress();
		IpAddress ServIP = sf::IpAddress(127, 0, 0, 1);

		listener.listen(SERVICE_PORT);
	}

	int process_messages(char* msg, size_t msg_size, size_t received, int number_connection) {
		if (received <= 0) {
			deleteClient(number_connection);
			return -1;
		}
		for (char* pPtr = msg; *pPtr != 0; pPtr++)
		{
			if (*pPtr == '\n' || *pPtr == '\r')
			{
				*pPtr = 0;
				break;
			}
		}
		// Пропускаем пустые строки
		if (msg[0] == 0)
			return 1;

		for (int i = 0; i < 1023 && msg[i + 1] != 0; i++) {
			if (msg[i] == ' ') {
				for (int j = i; j < 1023; j++) {
					msg[j] = msg[j + 1];
				}
				i--;
			}
			else break;
		}
		return 0;
	}

	void deleteClient(int numberConnection) {
		string addr_port = string(client[numberConnection].getRemoteAddress().toString()); addr_port += ':'; addr_port += string(to_string(client[numberConnection].getRemotePort()));
		for (auto it = connectionUser.begin(); it != connectionUser.end(); it++) {
			if (!strnicmp(addr_port.c_str(), (*it).c_str(), addr_port.size())) {
				connectionUser.erase(it);
				break;
			}
		}
		clientDataChanels[numberConnection].disconnect();
		client[numberConnection].disconnect();
		dataChanelOpen[numberConnection] = false;
		binaryMode[numberConnection] = false;
	}

	void addClient(int number_connection) {

		clientDataChanels[number_connection].disconnect();
		dataChanelOpen[number_connection] = false;
		binaryMode[number_connection] = false;
		string addr_port = string(client[number_connection].getRemoteAddress().toString()); addr_port += ':'; addr_port += string(to_string(client[number_connection].getRemotePort()));
		addr_port += " *...*";

		string Welcome_msg = "220 FTP_Semenov_Server";
		connectionUser.push_back(addr_port);
		MSG.push_back(string("SERVER: " + Welcome_msg));
		Welcome_msg += "\r\n";

		client[number_connection].send(Welcome_msg.c_str(), Welcome_msg.length());
		userName[number_connection] = "...";
		authorisation[number_connection] = false;
		anonymous[number_connection] = false;
		dataChanel[number_connection] = false;
		user[number_connection] = false;
		CurrentDirrectory[number_connection] = '/';
	}

	string USER(vector<string> words, int number_connection) {
		string status;
		string tmp;
		anonymous[number_connection] = false;
		authorisation[number_connection] = false;
		if (words.size() > 1) {
			tmp.clear();
			for (int i = 1; i < words.size(); i++) {
				if (i != 1) tmp += " ";
				tmp += words[i];
			}
			std::transform(tmp.begin(), tmp.end(), tmp.begin(), toupper);

			if (tmp == "FTP" || tmp == "ANONYMOUS") {
				anonymous[number_connection] = true;
				user[number_connection] = true;
				userName[number_connection] = "*ANONIMOUS*";
				status = "331 Anonimous acess allowed, send indenty(e-mail) as password.";
			}
			else {
				anonymous[number_connection] = false;
				userName[number_connection] = tmp;
				user[number_connection] = true;
				status = "531 Password required.";
			}
		}
		else {
			status = "501 Invalid number of parameters.";
			user[number_connection] = false;
		}
		return status;
	}

	string PASS(vector<string> words, int number_connection) {
		string status;
		string tmp;
		if (!user) {
			status = "503 Login with USER first.";
		}
		else {
			if (anonymous[number_connection]) {
				authorisation[number_connection] = true;
				status = "230 User logged in.";
			}
			else if (words.size() > 1) {
				std::transform(tmp.begin(), tmp.end(), tmp.begin(), toupper);
				if (anonymous[number_connection] && (tmp == "FTP" || tmp == "ANONYMOUS" || tmp == "")) {
					status = "230 User logged in.";
					user[number_connection] = true;
				}
				else {
					if (USERS.size()) {
						for (int i = 1; i < words.size(); i++) {
							if (i != 1) tmp += " ";
							tmp += words[i];
						}
						int i = 0;
						string tmp_us;
						for (auto it = USERS.begin(); it != USERS.end(); it++, i++) {
							tmp_us = (*it).login;
							std::transform(tmp_us.begin(), tmp_us.end(), tmp_us.begin(), toupper);
							if (tmp_us == userName[number_connection] && tmp == (*it).password) {
								status = "230 User logged it";
								anonymous[number_connection] = false;
								authorisation[number_connection] = true;
								break;
							}
						}
						if (!authorisation)
							status = "530 Not logged in.";
					}
					else
						status = "530 Not logged in.";
				}

			}
			else {
				status = "503 User cannot log in.";
				user[number_connection] = false;
				authorisation[number_connection] = false;
			}
		}
		return status;
	}

	string SYST() {
		return string("215 Unix emulator.");
	}

	string PWD(int number_connection) {
		string status;
		status = "257 \""; status += CurrentDirrectory[number_connection]; status += "\" is current directory";
		return status;
	}

	string CDUP(int number_connection) {
		string status;
		if (authorisation) {
			string path_previous = getPreviousPath(CurrentDirrectory[number_connection]);
			CurrentDirrectory[number_connection] = path_previous;
			status = "250 CDUP command successful";
		}
		else
			status = "500 Command not understood.";
		return status;
	}

	string CWD(vector<string> words, int number_connection) {
		string status;
		if (words.size() == 1) {
			status = "250 CWD command successful";
		}
		else {
			string tmp = words[1];
			string tmp_Cur = CurrentDirrectory[number_connection];
			if (isRootDirectory(tmp)) {
				CurrentDirrectory[number_connection] = '/';
				status = "250 CWD command successful";
				return status;
			}
			string path;
			if (isRootDirectory(tmp_Cur)) {
				deleteBeginSlash(tmp);
				path = tmp;
			}
			else {
				deleteBeginSlash(tmp);
				deleteBeginSlash(tmp_Cur);
				addEndSlach(tmp_Cur);
				path = tmp_Cur + tmp;
			}
			string previous = getPreviousPath(path);
			deleteBeginSlash(previous);
			ConvertToWindowsPath(previous);

			bool find = false;
			list_files = LIST_S(previous);
			for (auto it = list_files.begin(); it != list_files.end(); it++) {
				if (tmp == (*it).path) {
					status = "250 CWD command successful";
					CurrentDirrectory[number_connection] = '/' + tmp;
					find = true;
					break;
				}
			}
			if (!find)
				status = "550 The sytem cannot find is file specified";
			else
				status = "250 CWD command successful";
		}
		return status;
	}

	string TYPE(vector<string> words) {
		string status;
		string tmp;
		if (words.size() > 1) {
			tmp = words[1];
			std::transform(tmp.begin(), tmp.end(), tmp.begin(), toupper);
			if (tmp == 'I')
				status = "200 Type set to I";
			else if (tmp == 'A')
				status = "200 Type set to I";
			else
				status = "501 Parameters not understood";
		}
		else status = "501 Invalid number of parameters";
		return status;
	}

	string LIST(vector<string> words, int number_connection) {
		string status;
		if (authorisation[number_connection] == false)
			status = "530 Please login with User and PASS.";
		else {

			status = "150 Opening ";
			if (binaryMode[number_connection] == true)
				status += "Binary ";
			else
				status += "ASCII ";
			status += "mode data connection.";

			if (dataChanelOpen[number_connection] == false) {
				status += "\r\n425 Cannot open data connection.";
				return status;
			}

			list_files = LIST_S(CurrentDirrectory[number_connection]);
			cout << "Check list files on disk." << endl;
			Sleep(10);

			string str;
			for (auto it = list_files.begin(); it != list_files.end(); it++) {
				if ((*it).isDirectory) {
					str += string((*it).linux_rights + "   3 ftpadmin sedapps          0 ");
					str += (*it).time;
					str += ' ';
					str += ((*it).path + "\r\n");

				}
				else {
					char buffer[15];
					str += string((*it).linux_rights + "   1 ftpadmin   group            ");
					itoa((*it).sizeB, buffer, 10);
					str += buffer; str += ' '; str += (*it).time + ' '; str += ((*it).path + "\r\n");
				}
			}

			status = "125 Data connection already open; Transfer starting.";

			MSG.push_back(string("SERVER: " + status));

			status += "\r\n";
			client[number_connection].send(status.c_str(), status.size() + 1);

			//str = "-rwxrwxrwx    1 owner    group            46024 Apr 20  3:36 readme.txt\r\ndrwxr-xr-x    3 ftpadmin sedapps     16384 Jun 26  2016 pub\r\n";
			//доделать
			//send(SCLsPASSV[mumber_connection], str.c_str(), str.size(), 0);
			clientDataChanels[number_connection].send(str.c_str(), str.size());
			std::cout << "PASV mail send\n";

			status = "226 Data transfer";
			MSG.push_back(string("SERVER: " + status));

			status += "\r\n";
			client[number_connection].send(status.c_str(), status.size() + 1);
			//доделать
			//closePASVConnection[mumber_connection] = true;
			clientDataChanels[number_connection].disconnect();
			dataChanelOpen[number_connection] = false;
			status.clear();
		}
		return status;
	}

	string PASV(vector<string> words, int number_connection) {
		string status;
		if (authorisation[number_connection] == false)
			status = "530 Please login with User and PASS.";
		else {
			thread th_COMMUNICATION([&status](int number_connection) {
				TcpListener listener;
				listener.listen(0);
				int port = listener.getLocalPort();

				string buff = to_string(port / 256); // itoa(port / 256, buff, 10);
				string str = "227 Entering Passive Mode (127,0,0,1," + string(buff) + ',';
				buff = to_string(port % 256);  str += buff; str += ')';
				status = str;

				if (listener.accept(clientDataChanel[number_connection]) == sf::Socket::Done)
				{
					std::cout << "PASV " + clientDataChanel[number_connection].getRemoteAddress().toString() + ":" + to_string(clientDataChanel[number_connection].getRemotePort()) << endl;
					while (dataChanelOpen_g[number_connection] == true) {
					}
					cout << "PASV closed" << endl;

				}
				}, number_connection);
			while (status == "") {
				continue;
			}
			dataChanelOpen[number_connection] = true;
//			th.push_back(th_COMMUNICATION);
			th_COMMUNICATION.detach(); 
		}
		return status;
	}

	string PORT(vector<string> words, int number_connection) {
		string status;
		if (authorisation[number_connection] == false)
			status = "530 Please login with User and PASS.";
		else {
			
			if (words.size() == 1)
				status = "501 invalid number of parameters.";
			else {
				string _1, _2, _3, _4,_5,_6;
				int poz[6] = { 0 };
				int* it_poz = poz;
				string tmp = words[1];
				int i = 0;
				for (auto it = tmp.begin(); it != tmp.end(); it++,i++) {
					if ((*it) == ',') {
						*it_poz = i;
						it_poz++;
					}
					else if(i+1==tmp.size())
						*it_poz = i;

				}
				for (int i = 0; i < poz[0]; i++) 
					_1 += tmp[i];

				for (int i = poz[0]+1; i < poz[1]; i++)
					_2 += tmp[i];

				for (int i = poz[1] + 1; i < poz[2]; i++)
					_3 += tmp[i];

				for (int i = poz[2] + 1; i < poz[3]; i++)
					_4 += tmp[i];

				for (int i = poz[3] + 1; i < poz[4]; i++)
					_5 += tmp[i];

				for (int i = poz[4] + 1; i <= poz[5]; i++)
					_6 += tmp[i];
				IpAddress cl_IP(convertTo_Int(_1), convertTo_Int(_2), convertTo_Int(_3), convertTo_Int(_4));
				clientDataChanels[number_connection].disconnect();

				dataChanelOpen[number_connection] = true;
				thread th_COMMUNICATION([&status](int number_connection, IpAddress cl_IP,int port) {
				if (clientDataChanel[number_connection].connect(cl_IP, port) == sf::Socket::Done) {
					char msg[2048]; size_t received;
					while (dataChanelOpen_g[number_connection] == true) {

					}

					//clientDataChanel[number_connection].receive(msg, sizeof(msg), received);
				
				}
				}, number_connection, cl_IP,convertTo_Int(_5) * 256 + convertTo_Int(_6));
				th_COMMUNICATION.detach();
				status = "200 PORT commend successful.";
			}
		}
		return status;
	}

	string RETR(vector<string> words, int number_connection) {
		string status;
		if (authorisation[number_connection] == false)
			status = "530 Please login with User and PASS.";
		else {
			//проверить открыт ли канал данных
			list_files = LIST_S(CurrentDirrectory[number_connection]);
			bool find = false;
			string tmp = words[1];

			deleteBeginSlash(tmp);
			deleteEndSlash(tmp);
			bool file = true;
			for (auto it = list_files.begin(); it != list_files.end();it++) {
				if ((*it).path == tmp) {
					find = true;
					if ((*it).isDirectory)
						file = false;
					break;
				}
			}
			if(!find)
			status = "550 The system cannot find the file specified.";
			else {
				if (file) {
					status = "125 Data connection already open. Transfer starting.";
					MSG.push_back("SERVER: " + status);
					status += "\r\n";
					client[number_connection].send(status.c_str(), status.size()+1);
					string file;
					file = readFile(CurrentDirrectory[number_connection] + tmp);
					//client[number_connection].send(tmp);
					clientDataChanels[number_connection].send(file.c_str(), file.size());
					status = "226 Transfer complete.";
					clientDataChanels[number_connection].disconnect();
					dataChanelOpen[number_connection] = false;
				}
			}
		}
		return status;
	}

	string STOR(vector<string> words, int number_connection) {
		string status;
		ofstream fout(words[1]);
		if (fout.is_open()) {
			status = "150 Opening data channel for file ipload to server of \""; status += CurrentDirrectory[number_connection]+ words[1] +"\"";
			MSG.push_back("SERVER: " + status);
			status += "\r\n";
			client[number_connection].send(status.c_str(),status.size()+1);
			size_t received = 0;
			char msg[1024];

			clientDataChanels[number_connection].receive(msg,sizeof(msg),received);
			string file;
			
			for (int i = 0; i < received; i++) {
				if (msg[i] == '\r') {
					continue;
				}
				
				file += msg[i];
			}
			fout << file;

			status = "226 Successfully transferred \""; status += CurrentDirrectory[number_connection] + words[1] + "\"";

		}
		

		return status;
	}

	string QUIT(int number_connection) {
		string status = "221 Goodbye.\r\n";
		MSG.push_back(string("SERVER: " + status));
		client[number_connection].send(status.c_str(), status.size() + 1);
		deleteClient(number_connection);
		status = string();
		return status;
	}


	std::string readFile(const std::string& fileName) {
		/*std::ifstream input(fileName);
		string s;
		bool first = true;
		for (std::string line; getline(input, line); )
		{
			if (!first)
				s += '\n';
			else first = false;

			s += line;
		}

		*/

		std::ifstream ifs(fileName);
		std::string s;
		if (ifs.is_open())
		{
			s.assign((std::istreambuf_iterator<char>(ifs.rdbuf())), std::istreambuf_iterator<char>());
			std::cout << s;
			ifs.close();
		}
		for (auto it = s.begin(); it != s.end(); it++) {
			if (*it == '\n') {
				it = s.insert(it, '\r');
				it++;
			}
		}


		return s;
	}
};

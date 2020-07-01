#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include "FTPServ.h"
#include "Window.h"

using namespace std;
using namespace sf;




FTPServ FTP;
WindowFTP winFTP;

vector<thread> th;

int CONNECTION = 0;




void ACCEPT(int number_connection) {

	
	char msg[1024];size_t received;
	vector<string> words; string tmp;


	//добавить клиента в систему
	FTP.addClient(number_connection);

	while (true) {																								// Получаем и обрабатываем данные от клиента
		ZeroMemory(msg, sizeof(msg));
		words.clear();
		FTP.client[number_connection].receive(msg, sizeof(msg), received);

		int ans = FTP.process_messages(msg, sizeof(msg), received, number_connection);
		if (ans == -1) break;
		else if (ans == 1) continue;

		cout << "Received data : " << msg << endl;
		FTP.MSG.push_back(string("CLIENT: [" + FTP.client[number_connection].getRemoteAddress().toString() + ':' + to_string(FTP.client[number_connection].getRemotePort()) +"] " + string(msg)));

		ParsMessage(msg, sizeof(msg), words);

		if (words.size() >= 1)
			tmp = words[0];
		else	continue;
		std::transform(tmp.begin(), tmp.end(), tmp.begin(), toupper);

		string status;
		if (tmp == "USER")
			status = FTP.USER(words, number_connection);
		else if (tmp == "PASS")
			status = FTP.PASS(words, number_connection);
		else if (tmp == "SYST")
			status = FTP.SYST();
		else if (tmp == "TYPE")
			status = FTP.TYPE(words);
		else if (tmp == "PWD")
			status = FTP.PWD(number_connection);
		else if (tmp == "CWD")
			status = FTP.CWD(words, number_connection);
		else if (tmp == "CDUP")
			status = FTP.CDUP(number_connection);
		else if (tmp == "PASV")
			status = FTP.PASV(words, number_connection);
		else if (tmp == "PORT")
			status = FTP.PORT(words, number_connection);
		else if (tmp == "LIST")
			status = FTP.LIST(words, number_connection);
		else if (tmp == "RETR")
			status = FTP.RETR(words, number_connection);
		else if (tmp == "STOR")
			status = FTP.STOR(words, number_connection);
		else if (tmp == "QUIT")
			status = FTP.QUIT(number_connection);
		else
			status = "500 Command not understood.";
		
		if (status.size())
			FTP.MSG.push_back(string("SERVER: " + status));

		status += "\r\n";
		if(status.size())
			FTP.client[number_connection].send(status.c_str(), status.size());
		if (FTP.authorisation[number_connection] == true) {
			string addr_port = string(FTP.client[number_connection].getRemoteAddress().toString()); addr_port += ':'; addr_port += string(to_string(FTP.client[number_connection].getRemotePort()));
			for (auto it = FTP.connectionUser.begin(); it != FTP.connectionUser.end(); it ++ ) {
				if (!strnicmp(addr_port.c_str(), (*it).c_str(), addr_port.size())) {
					*it = addr_port + " " + FTP.userName[number_connection];
				}
			}
		}
	}
	FTP.deleteClient(number_connection);
}

int main() {
	cout << "\t\t\tCOMPLETE" << endl;
	HWND hConsole = GetConsoleWindow();//Если компилятор старый заменить на GetForegroundWindow()
	ShowWindow(hConsole, SW_HIDE);//собственно прячем оконо консоли

	setlocale(LC_ALL, "");							// Поддержка кириллицы в консоли Windows
	winFTP.setServFTP(FTP);
	

	thread th_COMMUNICATION([]() {
		while (true)
		{
			int number_connection = CONNECTION++;
			if (FTP.listener.accept(FTP.client[number_connection]) == sf::Socket::Done)
			{
				
				std::cout << "New connection received from " << FTP.client[number_connection].getRemoteAddress() << ":" << FTP.client[number_connection].getRemotePort() << endl;
				th.push_back(thread(ACCEPT, number_connection));
			}
		}
	});
	th_COMMUNICATION.detach();
	

	RenderWindow window(VideoMode(winFTP.screenSize.x, winFTP.screenSize.y), "FTP-SERVER");
	winFTP.setRenderWindow(window);

	while (winFTP.window->isOpen())
	{
		Event event;
		while (winFTP.window->pollEvent(event))
		{
			if (event.type == Event::Closed)
				winFTP.window->close();
		}


		winFTP.draw();
	}
	
	for (int i = 0; i < 256; i++) {
		FTP.client[i].disconnect();
	}
	return 0;
}


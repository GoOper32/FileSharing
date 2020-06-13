/*
* Простой TCP-сервер
* Поучает текстовую строку, преобразует ее в верхний регистр и возвращает
* клиенту результат.
*/
#include <iostream>
#include <Winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctime>
#include <string>
#pragma warning(disable: 4996)
#pragma comment(lib, "ws2_32.lib")

using namespace std;


// TCP-порт сервера
#define SERVICE_PORT 1500
int send_string(SOCKET s, const char* sString);
int convertTo_Int(string str) {
	int answer = 0;
	int multiplay = 10;
	if(str.size())
		answer = str[0] - '0';
	for(int i=1;i<str.size();i++){
		answer *= 10;
		answer += str[i] - '0';
	}
	return answer;
}

int summ(int a) {
	int answer = (a <= 0)? 0:1;

	for (int i = 0; i < a; i++) {
		answer += i;
	}
	return answer;
}

int main(void)
{
	char buffer[256];
	time_t now = time(0);
	string time_now;
	string str;
	tm* ltm = localtime(&now);

	SOCKET S; //дескриптор прослушивающего сокета
	SOCKET NS; //дескриптор присоединенного сокета
	sockaddr_in serv_addr;
	WSADATA wsadata;
	char sName[128];
	bool bTerminate = false;
	// Инициализируем библиотеку сокетов
	WSAStartup(MAKEWORD(2, 2), &wsadata);
	// Пытаемся получить имя текущей машины
	gethostname(sName, sizeof(sName));
	printf("\nServer host: %s\n", sName);
	// Создаем сокет
	// Для TCP-сокета указываем параметр SOCK_STREAM
	// Для UDP - SOCK_DGRAM
	if ((S = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET)
	{
		fprintf(stderr, "Can't create socket\n");
		exit(1);
	}
	// Заполняем структуру адресов
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	// Разрешаем работу на всех доступных сетевых интерфейсах,
	// в частности на localhost
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	// Обратите внимание на преобразование порядка байт
	serv_addr.sin_port = htons((u_short)SERVICE_PORT);
	// Связываем сокет с заданным сетевым интерфесом и портом
	if (bind(S, (sockaddr*)&serv_addr, sizeof(serv_addr)) == INVALID_SOCKET)
	{
		fprintf(stderr, "Can't bind\n");
		exit(1);
	}
	// Переводим сокет в режим прослушивания заданного порта
	// с максимальным количеством ожидания запросов на соединение 5
	if (listen(S, 5) == INVALID_SOCKET)
	{
		fprintf(stderr, "Can't listen\n");
		exit(1);
	}
	printf("Server listen on %s:%d\n",
		inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));
	// Основной цикл обработки подключения клиентов
	while (!bTerminate)
	{
		printf("Wait for connections.....\n");
		sockaddr_in clnt_addr;
		int addrlen = sizeof(clnt_addr);
		memset(&clnt_addr, 0, sizeof(clnt_addr));
		// Переводим сервис в режим ожидания запроса на соединение.
		// Вызов синхронный, т.е. возвращает управление только при
		// подключении клиента или ошибке
		NS = accept(S, (sockaddr*)&clnt_addr, &addrlen);
		if (NS == INVALID_SOCKET)
		{
			fprintf(stderr, "Can't accept connection\n");
			break;
		}
		// Получаем параметры присоединенного сокета NS и
		// информацию о клиенте
		addrlen = sizeof(serv_addr);
		getsockname(NS, (sockaddr*)&serv_addr, &addrlen);
		// Функция inet_ntoa возвращает указатель на глобальный буффер,
		// поэтому использовать ее в одном вызове printf не получится
		printf("Accepted connection on %s:%d ",
			inet_ntoa(serv_addr.sin_addr), ntohs(serv_addr.sin_port));
		printf("from client %s:%d\n",
			inet_ntoa(clnt_addr.sin_addr), ntohs(clnt_addr.sin_port));
		// Отсылаем вводную информацию о сервере
		send_string(NS, "* * * Welcome to simple UPCASE TCP-server * * *\r\n");
		//
		char sReceiveBuffer[1024] = { 0 };
		// Получаем и обрабатываем данные от клиента
		while (true)
		{
			int nReaded = recv(NS, sReceiveBuffer, sizeof(sReceiveBuffer) - 1, 0);
			// В случае ошибки (например, отсоединения клиента) выходим
			if (nReaded <= 0) break;
			// Мы получаем поток байт, поэтому нужно самостоятельно
			// добавить завержающий 0 для ASCII строки
			sReceiveBuffer[nReaded] = 0;
			// Отбрасываем символы превода строк
			for (char* pPtr = sReceiveBuffer; *pPtr != 0; pPtr++)
			{
				if (*pPtr == '\n' || *pPtr == '\r')
				{
					*pPtr = 0;
					break;
				}
			}
			// Пропускаем пустые строки
			if (sReceiveBuffer[0] == 0) continue;
			printf("Received data: %s\n", sReceiveBuffer);
			// Анализируем полученные команды или преобразуем текст в верхний регистр
			if (strcmp(sReceiveBuffer, "info") == 0)
			{
				send_string(NS, "Semenov Ivan Alexandrovi4.\r\n");
			}
			else if (strcmp(sReceiveBuffer, "task") == 0)
			{
				send_string(NS, "variant-18. The sum of all the numbers up to the one you entered.\r\n");
				while (true) {
					int nReaded = recv(NS, sReceiveBuffer, sizeof(sReceiveBuffer) - 1, 0);
					if (nReaded <= 0) break;
					sReceiveBuffer[nReaded] = 0;
					for (char* pPtr = sReceiveBuffer; *pPtr != 0; pPtr++)
					{
						if (*pPtr == '\n' || *pPtr == '\r')
						{
							*pPtr = 0;
							break;
						}
					}
					if (strcmp(sReceiveBuffer, "break") == 0) {
						break;
					}
					else {
						str = sReceiveBuffer;
						if (str != "") {
							itoa(summ(convertTo_Int(str)), buffer, 10);
							str = "The sum of all the numbers up to the one you entered = ";
							str += buffer;
							str += "\r\n";
							send_string(NS, str.c_str());
						}
					}
					
				}

			}
			else if (strcmp(sReceiveBuffer, "time") == 0)
			{
				ltm = localtime(&now);
				time_now = "System time : ";
				itoa(ltm->tm_mday, buffer, 10);
				time_now += buffer; time_now += "/";
				itoa(1 + ltm->tm_mon, buffer, 10);
				time_now += buffer; time_now += "/";
				itoa(1900 + ltm->tm_year, buffer, 10);
				time_now += buffer; time_now += " ";
				itoa(1 + ltm->tm_hour, buffer, 10);
				time_now += buffer; time_now += ":"; 
				itoa(ltm->tm_min, buffer, 10);
				time_now += buffer;
				time_now += "\r\n";
				send_string(NS, time_now.c_str());
			}
			else if (strcmp(sReceiveBuffer, "shutdown") == 0)
			{
				send_string(NS, "Server go to shutdown.\r\n");
				Sleep(200);
				bTerminate = true;
				break;
			}
			else if (strcmp(sReceiveBuffer, "exit") == 0)
			{
				send_string(NS, "Bye...\r\n");
				printf("Client initialize disconnection.\r\n");
				break;
			}
			else
			{
				// Преобразовываем строку в верхний регистр
				char sSendBuffer[1024];
				_snprintf(sSendBuffer, sizeof(sSendBuffer), "Server reply: %s\r\n",
					strupr(sReceiveBuffer));
				send_string(NS, sSendBuffer);
			}
		}
		// закрываем присоединенный сокет
		closesocket(NS);
		printf("Client disconnected.\n");
	}
	// Закрываем серверный сокет
	closesocket(S);
	// освобождаем ресурсы библиотеки сокетов
	WSACleanup();
	return 0;
}
// Функция отсылки текстовой ascii строки клиенту
int send_string(SOCKET s, const char* sString)
{
	return send(s, sString, strlen(sString), 0);
}

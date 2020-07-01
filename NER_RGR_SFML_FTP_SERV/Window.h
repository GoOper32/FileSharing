#pragma once
#include "Header.h"
#include "FTPServ.h"



class WindowFTP
{
	void PrintConnection(RenderWindow& window, unsigned startPos) {
		Font font; font.loadFromFile("Font/consola.ttf");
		for (int i = 0; i < FTP->connectionUser.size(); i++) {
			Text text(FTP->connectionUser[i].c_str(), font, 20);
			text.setColor(Color::Black);

			text.setPosition(startPos + 10, 40 + 40 * i);
			window.draw(text);
		}

	}

	void PrintMSG(RenderWindow& window, unsigned startPos) {
		Font font; font.loadFromFile("Font/consola.ttf");
		for (int i = 0; i < FTP->MSG.size(); i++) {
			
			Text text(FTP->MSG[i].c_str(), font, 10);
			if (!strnicmp(string("SERVER: ").c_str(), FTP->MSG[i].c_str(),size_t(7)))
				text.setColor(Color::Black);
			else {
				text.setColor(Color::Magenta);
			}

			text.setPosition(startPos + 10,  10 + 10 * i);
			window.draw(text);
		}
	}


public:
	FTPServ* FTP;

	RenderWindow* window;
	Vector2u screenSize;

	Color green;

	sf::RectangleShape rectangle_cnct; 
	sf::RectangleShape rectangle_main;

	Font font;
	Text text;
	Texture herotexture;
	Sprite connection_sprite;
	Text count_connectionText;

	void setServFTP(FTPServ& FTP) {
		this->FTP = &FTP;
	}

	WindowFTP() :screenSize(960, 540) {
		green = Color(59, 80, 58, 0);
		rectangle_cnct = sf::RectangleShape(sf::Vector2f(screenSize.x - (screenSize.x / 3) * 2, screenSize.y));
		rectangle_main = sf::RectangleShape(sf::Vector2f((screenSize.x / 3) * 2, screenSize.y));
		font.loadFromFile("Font/consola.ttf");

		text = Text("Connections:", font, 20);
		count_connectionText = Text('0', font, 20);

		rectangle_main.setFillColor(Color(204, 204, 204.0));	rectangle_main.setPosition(0, 0);
		rectangle_cnct.setFillColor(Color(153, 153, 153.0));	rectangle_cnct.setPosition((screenSize.x / 3) * 2, 0);
		text.setColor(Color::Black);
		text.setPosition((screenSize.x / 3) * 2 + 90, 10);
		herotexture.loadFromFile("Icons/connection.png");
		connection_sprite.setTexture(herotexture);
		connection_sprite.setPosition((screenSize.x / 10) * 9, 12);
		count_connectionText.setColor(Color::Black);
		count_connectionText.setPosition((screenSize.x / 10) * 9 + 30, 10);

	}
	
	void setRenderWindow(RenderWindow& window) {
		this->window = &window;
	}

	void draw() {
		window->clear();
		window->draw(rectangle_cnct);
		window->draw(rectangle_main);
		window->draw(text);
		window->draw(connection_sprite);
		PrintConnection(*window, (screenSize.x / 3) * 2);
		PrintMSG(*window, 10);
		count_connectionText.setString(to_string(FTP->connectionUser.size())); window->draw(count_connectionText);
		window->display();
	}
};


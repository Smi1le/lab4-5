#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <Windows.h>

using namespace sf;
using namespace std;

struct Files {
	string *files = new string[0];
	unsigned int nameSize = 0;
	unsigned int arrSize = 0;
	string path;
}typedef Files;

struct Pictures {
	Sprite *sprite = new Sprite;
	Texture *texture = new Texture;
	Texture *past = new Texture;
	Texture *next = new Texture;
	int numberPic = 0;
	bool error = true;
	bool ne = false;
	bool pr = false;
	string title;
}typedef Pictures;

float getFileSize(string path) {
	DWORD fileSize = 0;
	HANDLE file = CreateFile(path.c_str(),
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN,
		NULL);
	if (file == INVALID_HANDLE_VALUE)
		cout << "Не удается открыть файл" << endl;
	fileSize = GetFileSize(file, NULL);
	if (fileSize == INVALID_FILE_SIZE)
		cout << "?? ??????? ?????????? ?????? ?????" << endl;
	CloseHandle(file);
	return fileSize / 1024.0 / 1024.0;
}

bool checkImage(string fileName) {
	if (!strrchr(fileName.c_str(), '.')) {
		return false;
	}
	string listExtensions[] = { "jpg", "jpeg", "png", "gif", "bmp" };
	for (int i = 0; i < 5; i++) {
		if (fileName.substr(fileName.find_last_of(".") + 1) == listExtensions[i])
			return true;
	}
	return false;
}

void initPicture(Vector2u window_size, Files files, Pictures *pic, char diraction) {
	if (!(pic->numberPic < 0)) {
		if (!pic->error) {
			delete(pic->sprite);
		}
		Image *image = new Image;
		string path = files.path + files.files[pic->numberPic];
		if (getFileSize(path) > 10) {
			std::cout << "error (too big size) with: \n" << files.path + files.files[pic->numberPic] << '\n';
			pic->error = true;
		}
		if (!(image->loadFromFile(files.path + files.files[pic->numberPic]))) {
			std::cout << "error with: \n" << files.path + files.files[pic->numberPic] << '\n';
			pic->error = true;
		}
		else if (image->getSize().x > 16000 || image->getSize().y > 16000) {
			pic->error = true;
		}
		else {
			pic->error = false;
		}
		diraction = 0;
		if (!pic->error) {
			if (diraction == 1) {
				pic->next = pic->texture;
				pic->ne = true;
			}
			else if (diraction == 2) {
				pic->past = pic->texture;
				pic->pr = true;
			}
		}
		if (diraction == 1 && pic->pr) {
			pic->texture = pic->past;
			pic->pr = false;
		}
		else if (diraction == 2 && pic->ne) {
			pic->texture = pic->next;
			pic->ne = false;
		}
		else {
			delete(pic->texture);
			pic->texture = new Texture;
			pic->texture->loadFromImage(*image);
			delete(image);
		}
		pic->sprite = new Sprite;
		pic->sprite->setPosition(0, 0);
		pic->sprite->setTexture(*(pic->texture));
		if (image->getSize().x > 1000 && image->getSize().y > 650) {
			pic->sprite->setScale(float(window_size.x) / pic->texture->getSize().x, float(window_size.y) / pic->texture->getSize().y);
		}
		pic->sprite->setOrigin(pic->texture->getSize().x / 2.0f, pic->texture->getSize().y / 2.0f);
		pic->sprite->setPosition(window_size.x / 2.0f, window_size.y / 2.0f);
		pic->title = string(files.files[pic->numberPic]);
	}
}

Files list(string &pathOld) {
	Files files;
	string path = pathOld +string("*");
	files.path = pathOld;
	int i = 0;
	WIN32_FIND_DATA fileData;	
	HANDLE file = FindFirstFile(path.c_str(), &fileData);
	if (file != INVALID_HANDLE_VALUE) {
		do {
			if (!(fileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)) {
				files.arrSize++;
				if (strlen(fileData.cFileName) > (files.nameSize)) {
					files.nameSize = strlen(fileData.cFileName);
				}
			}
		} while (FindNextFile(file, &fileData));

		files.files = new string[files.arrSize];
		i = 0;
		file = FindFirstFile(path.c_str(), &fileData);
		do {
			if (!(fileData.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)) {
				if (checkImage(fileData.cFileName)) {
					files.files[i] = fileData.cFileName;
					i++;
				}
			}
		} while (FindNextFile(file, &fileData));
	}
	return files;
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(1000, 650), "SFML works!");

	string path = "C:\\Users\\asus1\\Desktop\\downloaded_images\\";

	Vector2u windowSize = window.getSize();
	Files files = list(path);
	Pictures pictures;

	pictures.texture->loadFromFile(path + files.files[0]);
	pictures.sprite->setTexture(*pictures.texture);

	Texture arrow;
	arrow.loadFromFile("images/arrows.png");

	Sprite left, right;
	left.setTexture(arrow);
	right.setTexture(arrow);
	left.setTextureRect(IntRect(0, 0, 75, 50));
	right.setTextureRect(IntRect(75, 0, 75, 50));
	left.setPosition(415, 600);
	right.setPosition(510 , 600);

	Font font;
	font.loadFromFile("image/Monoid.ttf");
	Text error("ERROR", font, 60);
	error.setColor(Color(255, 0, 0));
	error.setOrigin(strlen("ERROR") / 2 * 60, 10);
	int i = 0;
	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			Vector2i localPosition = Mouse::getPosition(window);
			Vector2i pixelPos = Mouse::getPosition(window);
			Vector2f pos = window.mapPixelToCoords(pixelPos);

			if (event.type == sf::Event::Closed)
				window.close();
			if ((event.type == Event::MouseButtonReleased) && (event.key.code == Mouse::Left)) {
				if (right.getGlobalBounds().contains(pos.x, pos.y)) {
					if (pictures.numberPic == 0) {
						pictures.numberPic = files.arrSize;
						i = 0;
					}
					pictures.numberPic--;
					initPicture(windowSize, files, &pictures, 1);
				}
				else if (left.getGlobalBounds().contains(pos.x, pos.y)) {
					if (pictures.numberPic + 1 == files.arrSize) {
						pictures.numberPic = 0;
						i = files.arrSize;
					}
					else {
						pictures.numberPic++;
					}
					initPicture(windowSize, files, &pictures, 2);
				}
			}
		}

		window.clear();
		window.draw(*pictures.sprite);
		window.draw(left);
		window.draw(right);
		window.display();
	}

	return 0;
}
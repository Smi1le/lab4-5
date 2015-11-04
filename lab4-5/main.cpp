#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <Windows.h>

using namespace sf;
using namespace std;

struct Files {
	string *files = new string[0];
	unsigned int name_size = 0;
	unsigned int arr_size = 0;
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

float get_file_size(string path) {
	LPCTSTR file = path.c_str();
	DWORD file_size = 0;
	HANDLE h_File = CreateFile(file,
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_FLAG_SEQUENTIAL_SCAN,
		NULL);
	if (h_File == INVALID_HANDLE_VALUE)
		cout << "Не удается открыть файл";
	file_size = GetFileSize(h_File, NULL);
	if (file_size == INVALID_FILE_SIZE)
		cout << "?? ??????? ?????????? ?????? ?????";
	CloseHandle(h_File);
	return file_size / 1024.0 / 1024.0;
}

bool check_image(string file_name) {
	if (!strrchr(file_name.c_str(), '.')) {
		return false;
	}
	string list_extensions[] = { "jpg", "jpeg", "png", "gif", "bmp" };
	for (int i = 0; i < 5; i++) {
		if (file_name.substr(file_name.find_last_of(".") + 1) == list_extensions[i])
			return true;
	}
	return false;
}

void init_picture(Vector2u window_size, Files files, Pictures *pic, char diraction) {
	if (!(pic->numberPic < 0)) {
		if (!pic->error) {
			delete(pic->sprite);
		}
		Image *image = new Image;
		string path = files.path + files.files[pic->numberPic];
		if (get_file_size(path) > 10) {
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
		pic->sprite->setOrigin(pic->texture->getSize().x / 2.0, pic->texture->getSize().y / 2.0);
		pic->sprite->setPosition(window_size.x / 2.0, window_size.y / 2.0);
		pic->title = string(files.files[pic->numberPic]);
	}
}

Files list(string &path_b) {
	Files files;
	string path = path_b +string("*");
	files.path = path_b;
	int i = 0;
	WIN32_FIND_DATA file_data;	
	HANDLE d_file = FindFirstFile(path.c_str(), &file_data);
	if (d_file != INVALID_HANDLE_VALUE) {
		do {
			if (!(file_data.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)) {
				files.arr_size++;
				if (strlen(file_data.cFileName) > (files.name_size)) {
					files.name_size = strlen(file_data.cFileName);
				}
			}
		} while (FindNextFile(d_file, &file_data));

		files.files = new string[files.arr_size];
		i = 0;
		d_file = FindFirstFile(path.c_str(), &file_data);
		do {
			if (!(file_data.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)) {
				if (check_image(file_data.cFileName)) {
					files.files[i] = file_data.cFileName;
					i++;
				}
			}
		} while (FindNextFile(d_file, &file_data));
	}
	return files;
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(1000, 650), "SFML works!");

	string path = "C:\\Users\\asus1\\Desktop\\downloaded_images\\";

	Vector2u window_size = window.getSize();
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
	
	cout << "window_size.x = " << window_size.x << endl;
	cout << "window_size.y = " << window_size.y << endl;
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
					cout << "pictures.numberPic = " << pictures.numberPic << endl;
					if (pictures.numberPic == 0) {
						pictures.numberPic = files.arr_size;
						i = 0;
					}
					pictures.numberPic--;
					/*Image *image = new Image;
					i++;
					image->loadFromFile(path + files.files[i]);
					pictures.sprite->setOrigin( image->getSize().x / 2.0, image->getSize().y / 2.0);
					pictures.sprite->setPosition(window_size.x / 2.0, window_size.y / 2.0);*/
					cout << " files.files[pictures.numberPic] =  " << files.files[pictures.numberPic] << endl;
					init_picture(window_size, files, &pictures, 1);
				}
				else if (left.getGlobalBounds().contains(pos.x, pos.y)) {
					if (pictures.numberPic + 1 == files.arr_size) {
						pictures.numberPic = 0;
						i = files.arr_size;
					}
					else (pictures.numberPic++);
					/*Image *image = new Image;
					i++;
					image->loadFromFile(path + files.files[i]);
					pictures.sprite->setOrigin(window_size.x / 2.0, window_size.y / 2.0);
					pictures.sprite->setPosition(window_size.x / 2.0, window_size.y / 2.0);*/
					cout << " files.files[pictures.numberPic] =  " << files.files[pictures.numberPic] << endl;
					init_picture(window_size, files, &pictures, 2);
				}
				if (event.type == Event::MouseMoved) {
					if (left.getGlobalBounds().contains(pos.x, pos.y)) {
						left.setColor(Color(255, 255, 255, 100));
					}
					else {
						left.setColor(Color(255, 255, 255, 255));
					}
					cout << "right.getPosition.x = " << right.getPosition().x << endl;
					cout << "right.getPosition.y = " << right.getPosition().y << endl;
					if (right.getGlobalBounds().contains(pos.x, pos.y)) {
						right.setColor(Color::Green);
					}
					else {
						right.setColor(Color::White);
					}
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
#include <iostream>
#include <fstream>
#define _USE_MATH_DEFINES
#include <math.h>
#include "SFML/Graphics.hpp"
//#include "NumColor.hpp"

using namespace std;
using namespace sf;

#define guide "Используйте \"Reverse_Fisheye_Project.exe <k> [f] <image_path>\".\n"

class NumColor : public Color {
public:
	NumColor();

	NumColor(Color c);

	NumColor(Uint8 r1, Uint8 g1, Uint8 b1, Uint8 a1);
	NumColor operator*(const double& k) const;
};


NumColor::NumColor() {
	r = 0; g = 0; b = 0; a = 0;
}

NumColor::NumColor(Color c) {
	r = c.r;
	g = c.g;
	b = c.b;
	a = c.a;
}

NumColor::NumColor(Uint8 r1, Uint8 g1, Uint8 b1, Uint8 a1) {
	r = r1;
	g = g1;
	b = b1;
	a = a1;
}

NumColor NumColor::operator*(const double& k) const {
	return NumColor(r * k, g * k, b * k, a);
}


Color interpolation(double x, double y, Image image) {
	NumColor pixel;
	double x1 = floor(x), x2 = ceil(x), y1 = floor(y), y2 = ceil(y);
	if (x1 == x2 && y1 == y2)
		return image.getPixel(x, y);
	if (x1 == x2) {
		pixel = NumColor(image.getPixel(x, y1)) * ((y2 - y) / (y2 - y1)) + NumColor(image.getPixel(x, y2)) * ((y - y1) / (y2 - y1));
		return pixel;
	}
	if (y1 == y2) {
			pixel = NumColor(image.getPixel(x1, y)) * ((x2 - x) / (x2 - x1)) + NumColor(image.getPixel(x2, y)) * ((x - x1) / (x2 - x1));
		return pixel;
	}

	NumColor r1 = NumColor(image.getPixel(x1, y1)) * ((x2 - x) / (x2 - x1)) + NumColor(image.getPixel(x2, y1)) * ((x - x1) / (x2 - x1));
	NumColor r2 = NumColor(image.getPixel(x1, y2)) * ((x2 - x) / (x2 - x1)) + NumColor(image.getPixel(x2, y2)) * ((x - x1) / (x2 - x1));
	pixel = r1 * ((y2 - y) / (y2 - y1)) + r2 * ((y - y1) / (y2 - y1));
	return pixel;
}

int main(int argc, char** argstr) {
	setlocale(LC_ALL, "RUS");
	double k;	// Интенсивность коррекции.
	double f;	// Фокусное расстояние, по умолчанию - "чтобы экватор сферы был вписан в наименьший размер изображения".

	string imagePath, outImagePath;
	bool silent = 0;
	if (argc == 1) {
		cout << "Введите коэффициент k, названия входного и выходного изображений - {k} {Название входного, без пробелов и с расширением} {Название выходного}:\n";
		cin >> k >> imagePath >> outImagePath;
	}
	else {
		k = atoi(argstr[1]) / 100.0;
		imagePath = argstr[2];
		outImagePath = argstr[3];
		if (argc == 5)
			silent = atoi(argstr[4]);
		cout << argstr[1];
		cout << k << endl;
	}

	f = 0.0;

	// Если нет '\', указано лишь имя файла. 
	/*if (!imagePath.find('\\')) {
		string tempPath = relativePath;
		tempPath += imagePath;
		imagePath = tempPath;
	}*/

	Image inImage;
	if (!inImage.loadFromFile(imagePath)) {
		cout << "Не удалось считать изображение. " << guide;
		system("pause");
		return 0;
	}

	Vector2u imgSize = inImage.getSize();

	Texture ImgTxtr;
	ImgTxtr.loadFromImage(inImage);
	Sprite ImgSprt;
	ImgSprt.setTexture(ImgTxtr);

	RenderWindow in_window;
	if (!silent) {
		// Вывод изображения в окне.
		//in_window.setSize({ imgSize.x, imgSize.y });
		in_window.create(VideoMode(imgSize.x, imgSize.y), "Source");
		in_window.draw(ImgSprt);
		in_window.display();
	}

	int pivotX = imgSize.x / 2;
	int pivotY = imgSize.y / 2;
	double thetaMax = M_PI / 4;
	double theR;

	theR = min(pivotX, pivotY);
	
	if (f == 0.0) {
		if (k == 0.0) f = theR / thetaMax;
		else if (k > 0.0) f = theR * k / tan(k * thetaMax);
		else f = theR * k / sin(k * thetaMax);
	}

	Image outImage;	// Итоговое изображение.
	outImage.create(imgSize.x, imgSize.y);

	for (int x = 0; x < imgSize.x; x++) {
		for (int y = 0; y < imgSize.y; y++) {
			int xx = x - pivotX;
			int yy = y - pivotY;
			double alpha, r, dist = sqrt(xx * xx + yy * yy);
			alpha = atan2((double)yy, (double)xx);

			double theta;
			r = dist;
			if (k == 0.0) theta = r / f;
			else if (k < 0.0) theta = asin(r*k/f)/k;
			else theta = asin(r * k / f) / k;

			double sourceX = pivotX + theta * f * cos(alpha);
			double sourceY = pivotY + theta * f * sin(alpha);

			if (sourceX < 0 || sourceX >= imgSize.x - 1 || sourceY < 0 || sourceY >= imgSize.y - 1)
				continue;

			/*for (int i = 0; i <= 2; i++)
				outImage(x, y)[i] = inImage(round(sourceX), round(sourceY))[i];*/
			outImage.setPixel(x, y, interpolation(sourceX, sourceY, inImage));
		}
	}

	if (!silent) {
		// Вывод изображения в окне.

		RenderWindow out_window(VideoMode(imgSize.x, imgSize.y), "Result");

		ImgTxtr.loadFromImage(outImage);
		ImgSprt.setTexture(ImgTxtr, true);

		out_window.draw(ImgSprt);
		out_window.display();


		while (out_window.isOpen())
		{
			Event event;
			while (out_window.pollEvent(event))
			{
				if (event.type == Event::Closed) {
					in_window.close();
					out_window.close();
				}
			}
			while (in_window.pollEvent(event))
			{
				if (event.type == Event::Closed) {
					in_window.close();
					out_window.close();
				}
			}
		}
	}

	outImage.saveToFile(outImagePath);
}
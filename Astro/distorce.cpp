/*
Тисленков Сергей - Группа 6209.
Учебная практика 2019.
--------------------------------------------------------------------------------
Написать программу, которая переводит изображение из формата "рыбьего глаза"
с заданным значением параметра рыбьего глаза k в плоское изображение.
*/

#include <iostream>
#include <fstream>
#include <math.h>
#include <SFML/Graphics.hpp>
//#include "NumColor.hpp"

using namespace std;
using namespace sf;

#define guide "Используйте \"Reverse_Fisheye_Project.exe <k> [f] <image_path>\".\n"
#define PI 3.1415926536



class NumColor : public Color {
public:
	NumColor() {
		r = 0; g = 0; b = 0; a = 0;
	}

	NumColor(Color c) {
		r = c.r;
		g = c.g;
		b = c.b;
		a = c.a;
	}

	NumColor(Uint8 r1, Uint8 g1, Uint8 b1, Uint8 a1) {
		r = r1;
		g = g1;
		b = b1;
		a = a1;
	}
	NumColor operator*(const double& k) const {
		return NumColor(r * k, g * k, b * k, a);
	}
};



double sign(double x) {
	return (x >= 0.0 ? 1.0 : -1.0);
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

int main(int argc, char** argv) {
	setlocale(LC_ALL, "RUS");
	double k;	// Интенсивность коррекции.
	double f;	// Фокусное расстояние, по умолчанию - "чтобы экватор сферы был вписан в наименьший размер изображения".

	if (argc < 3 || argc > 4) {
		cout << "Неверные параметры. " << guide;
		return 0;
	}

	string relativePath = argv[0];	// Путь до программы.
	relativePath.erase(relativePath.length() - 27);
	string inK = argv[1];	// Пользовательский ввод.
	k = stod(inK);
	string inFocal = "0,0";
	string imagePath;

	switch (argc) {
	case 3:
		imagePath = argv[2];
		break;
	case 4:
		inFocal = argv[2];
		if (!isdigit(inFocal[0])) {
			cout << "Неверные параметры. " << guide;
			return 0;
		}
		imagePath = argv[3];
	}
	if (k > 1.0 || k < -1.0) {
		cout << "Неверные параметры. " << guide;
		return 0;
	}

	f = stod(inFocal);

	// Если нет '\', указано лишь имя файла. 
	if (!imagePath.find('\\')) {
		string tempPath = relativePath;
		tempPath += imagePath;
		imagePath = tempPath;
	}

	ifstream file(imagePath);
	if (!file.is_open()) {
		cout << "Не удалось считать изображение. " << guide;
		return 0;
	}
	else file.close();

	Mat_<Vec3b> inImage;
	inImage = imread(imagePath, IMREAD_COLOR);   // Считывание изображения.
	// Вывод изображения в окне.
	namedWindow("Source", WINDOW_AUTOSIZE);
	imshow("Source", inImage);

	int pivotX = inImage.rows / 2;
	int pivotY = inImage.cols / 2;
	double theta = PI / 4;
	double theR;

	if (pivotX >= pivotY)
		theR = pivotY;
	else theR = pivotX;
	
	if (f == 0.0) {
		if (k == 0.0) f = theR / theta;
		else if (k > 0.0) f = theR * k / tan(k * theta);
		else f = theR * k / sin(k * theta);
	}

	Mat_<Vec3b> outImage(inImage.rows, inImage.cols, CV_8UC3);	// Итоговое изображение.

	for (int x = 0; x < outImage.rows; x++) {
		for (int y = 0; y < outImage.cols; y++) {
			int xx = x - pivotX;
			int yy = y - pivotY;
			double alpha, r, dist = sqrt(xx * xx + yy * yy);
			alpha = atan2((double)xx, (double)yy);

			double phi = atan2(f, dist);
			if (k == 0.0) r = f * phi;
			else if (k < 0.0) r = f * sin(phi * k) / k;
			else r = f * tan(phi * k) / k;

			double sourceX = pivotX + r * cos(alpha);
			double sourceY = pivotY + r * sin(alpha);

			if (sourceX < 0 || sourceX >= outImage.rows - 1 || sourceY < 0 || sourceY >= outImage.cols - 1)
				continue;

			/*for (int i = 0; i <= 2; i++)
				outImage(x, y)[i] = inImage(round(sourceX), round(sourceY))[i];*/
			outImage(x, y) = interpolation(sourceX, sourceY, inImage);
		}
	}

	// Вывод изображения в окне.
	namedWindow("Result", WINDOW_AUTOSIZE);
	imshow("Result", outImage);
	waitKey(0);
	imwrite(relativePath + "outImage.jpg", outImage);
}
#include <iostream>
#include <fstream>
#define _USE_MATH_DEFINES
#include <math.h>
#include "SFML/Graphics.hpp"
//#include "NumColor.hpp"

using namespace std;
using namespace sf;

#define guide "Используйте \"Reverse_Fisheye_Project.exe <k> [f] <image_path>\".\n"

inline double sqr(double x) { return x * x; }

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

Color interpolation(double x, double y, const Image& image) {
	NumColor pixel;
	double x1 = floor(x), x2 = ceil(x), y1 = floor(y), y2 = ceil(y);
	if (x1 == x2 && y1 == y2)
		return image.getPixel(x, y);
	if (x1 == x2) {
		pixel = NumColor(image.getPixel(x, y1)) * (y2 - y) + NumColor(image.getPixel(x, y2)) * (y - y1);
		return pixel;
	}
	if (y1 == y2) {
			pixel = NumColor(image.getPixel(x1, y)) * (x2 - x) + NumColor(image.getPixel(x2, y)) * (x - x1);
		return pixel;
	}

	NumColor r1 = NumColor(image.getPixel(x1, y1)) * (x2 - x) + NumColor(image.getPixel(x2, y1)) * (x - x1);
	NumColor r2 = NumColor(image.getPixel(x1, y2)) * (x2 - x) + NumColor(image.getPixel(x2, y2)) * (x - x1);
	pixel = r1 * ((y2 - y) / (y2 - y1)) + r2 * ((y - y1) / (y2 - y1));
	return pixel;
}

Color test_color(255, 0, 255, 255);

// Проверить, насколько правильно искривили; изображение передаётся по константной ссылке
double test_distorce(const Image& img, int sizex, int sizey) {
	int sumx(0), sumy(0), sumx2(0), sumxy(0), cnt = 0;
	vector<Vector2u> coords;
	for (int x = 0; x < sizex; x++) {
		for (int y = 0; y < sizey; y++) {
			int r = abs(img.getPixel(x, y).r - test_color.r);
			int g = abs(img.getPixel(x, y).g - test_color.g);
			int b = abs(img.getPixel(x, y).b - test_color.b);
			if (r + g + b < 30) {
				++cnt;
				sumx += x;
				sumx2 += x * x;
				sumxy += x * y;
				sumy += y;
				coords.emplace_back(x, y);
			}
		}
	}

	if (cnt < 5) {
		cerr << "Точек меньше 5";
		if (cnt == 0) {
			cerr << "ТОЧЕК НЕТ";
			return 1e8;
		}
	}

	sumx /= cnt;
	sumy /= cnt;
	sumx2 /= cnt;
	sumxy /= cnt;

	if (sumx * sumx == sumx2)
		if (sumxy == sumx * sumy)
			return 1e8;
		return 0;

	double a = double(sumxy - sumx * sumy) / double(sumx2 - sumx * sumx);
	double b = sumy - a * sumx;
	double ans(0);

	for (auto i : coords) {
		ans += sqr(a * i.x + b - i.y);
	}

	return ans;
}

int main(int argc, char** argstr) {
	setlocale(LC_ALL, "RUS");
	double k;	// Интенсивность коррекции.
	double f;	// Фокусное расстояние, по умолчанию - "чтобы экватор сферы был вписан в наименьший размер изображения".


    string imagePath, outImagePath;
	bool silent = 0;
	if (argc == 1) {
		cout << "Введите коэффициент k в процентах, названия входного и выходного изображений - {k} {Название входного, без пробелов и с расширением} {Название выходного}:\n";
		cin >> k >> imagePath >> outImagePath;
	}
	else {
		k = atoi(argstr[1]);
		imagePath = argstr[2];
		outImagePath = argstr[3];
		if (argc == 5)
			silent = atoi(argstr[4]);
		cout << argstr[1] << " ";
		cout << k << endl;
	}

	
	k=k/100.;

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

	// Вывод изображения в окне.
	RenderWindow in_window(VideoMode(imgSize.x, imgSize.y), "Source");

	Texture ImgTxtr;
	ImgTxtr.loadFromImage(inImage);
	Sprite ImgSprt;
	ImgSprt.setTexture(ImgTxtr);
	
	in_window.draw(ImgSprt);
	in_window.display();

	int pivotX = imgSize.x / 2;
	int pivotY = imgSize.y / 2;
	double theta = M_PI / 4;
	double theR;

	theR = min(pivotX, pivotY);
	
	if (f == 0.0) {
		if (k == 0.0) f = theR / theta;
		else if (k > 0.0) f = theR * k / tan(k * theta);
		else{ f = theR * k / sin(k * theta); if(k<-1) f*=fabs(k);}
	}

	Image outImage;	// Итоговое изображение.
	outImage.create(imgSize.x, imgSize.y);

	for (int x = 0; x < imgSize.x; x++) {
		for (int y = 0; y < imgSize.y; y++) {
			int xx = x - pivotX;
			int yy = y - pivotY;
			double alpha, r, dist = sqrt(xx * xx + yy * yy);
			alpha = atan2((double)yy, (double)xx);

			double phi = dist / f;
			if (k == 0.0) r = f * phi;
			else if (k < 0.0) r = f * sin(phi * k) / k;
			else r = f * tan(phi * k) / k;

			double sourceX = pivotX + r * cos(alpha);
			double sourceY = pivotY + r * sin(alpha);

			if (sourceX < 0 || sourceX >= imgSize.x - 1 || sourceY < 0 || sourceY >= imgSize.y - 1)
				continue;

			outImage.setPixel(x, y, interpolation(sourceX, sourceY, inImage));
		}
	}

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

	outImage.saveToFile(outImagePath);
}
#include <iostream>
#include <fstream>
#define _USE_MATH_DEFINES
#include <math.h>
#include "SFML/Graphics.hpp"
//#include "NumColor.hpp"

using namespace std;
using namespace sf;

#define guide "����������� \"Reverse_Fisheye_Project.exe <k> [f] <image_path>\".\n"

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

double ptgui(double r,void* param){
 double k=*((double*)param);
 double f=*((double*)param+1); 
 if (k == 0.0) return r;
 else if (k < 0.0) return f*asin(r*k/f)/k;
 else return f*atan(r * k / f) / k; 
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
//	setlocale(LC_ALL, "RUS");
	double k;	// ������������� ���������.
	double f;	// �������� ����������, �� ��������� - "����� ������� ����� ��� ������ � ���������� ������ �����������".

	string imagePath, outImagePath;
//	cout << "Tralala!\n";
	if (argc == 1) {
		cout << "������� ����������� k, �������� �������� � ��������� ����������� - {k} {�������� ��������, ��� �������� � � �����������} {�������� ���������}:\n";
		cin >> k >> imagePath >> outImagePath;
	}
	else {
		k = atoi(argstr[1]) / 100.0;
		imagePath = argstr[2];
		outImagePath = argstr[3];
		cout << argstr[1];
		cout << k << endl;
	}

	f = 0.0;

	// ���� ��� '\', ������� ���� ��� �����. 
	/*if (!imagePath.find('\\')) {
		string tempPath = relativePath;
		tempPath += imagePath;
		imagePath = tempPath;
	}*/

	Image inImage;
	if (!inImage.loadFromFile(imagePath)) {
		cout << "�� ������� ������� �����������. " << guide;
		system("pause");
		return 0;
	}

	Vector2u imgSize = inImage.getSize();

	Texture ImgTxtr;
	ImgTxtr.loadFromImage(inImage);
	Sprite ImgSprt;
	ImgSprt.setTexture(ImgTxtr);

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
 double param[2]={f,k};

 Image outImage;	// �������� �����������.
 outImage.create(imgSize.x, imgSize.y);

 for (int x = 0; x < imgSize.x; x++) {
  for (int y = 0; y < imgSize.y; y++) {
   int xx = x - pivotX;
   int yy = y - pivotY;
   double r = sqrt(xx * xx + yy * yy),novr;
   double sinus,cosinus;

   novr=ptgui(r,param);   
   double sourceX = pivotX+novr*cosinus;
   double sourceY = pivotY+novr*sinus;
   if (sourceX < 0 || sourceX >= imgSize.x - 1 || sourceY < 0 || sourceY >= imgSize.y - 1)
    continue; // ����� ������������!

			outImage.setPixel(x, y, interpolation(sourceX, sourceY, inImage));
		}
	}
	outImage.saveToFile(outImagePath);
}
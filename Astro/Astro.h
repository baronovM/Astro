#pragma once
#include <fstream>
#include <limits>
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include <iomanip>
#include <filesystem>
#include <sstream>
#include <vector>
#include "SFML/Graphics.hpp"

using namespace std;
using namespace sf;

#define NUMCOEF 3
#define THRESHOLD 30
#define LOWER_LIMIT 0.6
#define UPPER_LIMIT 1.4

typedef Vector2<double> Vector2d;

inline double sqr(double x) { return x * x; }

class NumColor : public Color {
public:
	NumColor();

	NumColor(Color c);

	NumColor(Uint8 r1, Uint8 g1, Uint8 b1, Uint8 a1);
	NumColor operator*(const double& k) const;
	int operator-(const NumColor& other) const;
};


class SmartImage : public Image {
public:
	vector<vector<double>> precalc;
	int pivotX, pivotY, theR;
	SmartImage();
	SmartImage(string filePath);

	SmartImage(Vector2u si);
	void initNewR(const double coef[NUMCOEF]);
};

void roundArr(double c[NUMCOEF]);
double binpow(double x, int n);
Color interpolation(double x, double y, const SmartImage* image);
double test_distorce(const SmartImage* img, const Color& test_color);
double func(double r, const double c[NUMCOEF]);
double cont_test_sign(double r_max, const double c[NUMCOEF]);
SmartImage* distorce(const SmartImage* inImage, const double coef[NUMCOEF]);
SmartImage* distorce_dirch(const SmartImage* inImage, double f, double k);




#pragma once
#include <iostream>
#include <fstream>
#define _USE_MATH_DEFINES
#include <math.h>
#include "SFML/Graphics.hpp"

using namespace std;
using namespace sf;

#define NUMCOEF 3

inline double sqr(double x) { return x * x; }


class NumColor : public Color {
public:
	NumColor();

	NumColor(Color c);

	NumColor(Uint8 r1, Uint8 g1, Uint8 b1, Uint8 a1);
	NumColor operator*(const double& k) const;
};


class PlanImage : public Image {
public:
	int pivotX, pivotY, theR;
	PlanImage(string filePath);

	PlanImage(Vector2u si);
};

double binpow(double x, int n);
Color interpolation(double x, double y, const PlanImage& image);
double test_distorce(const PlanImage& img, const Color& test_color);
PlanImage* distorce(const PlanImage& inImage, double f, double k, double coef[NUMCOEF]);
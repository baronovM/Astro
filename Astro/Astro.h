#pragma once
#include <iostream>
#include <fstream>
#include <memory>
#define _USE_MATH_DEFINES
#include <math.h>
#include "SFML/Graphics.hpp"

using namespace std;
using namespace sf;

#define NUMCOEF 3
#define THRESHOLD 60
#define LOWER_LIMIT 0.2
#define UPPER_LIMIT 2.0
#define	MIN_DIFF_ANGLE M_PI / 12

inline double sqr(double x) { return x * x; }


class NumColor : public Color {
public:
	NumColor();

	NumColor(Color c);

	NumColor(Uint8 r1, Uint8 g1, Uint8 b1, Uint8 a1);
	NumColor operator*(const double& k) const;
	Uint8 operator-(const NumColor& other) const;
};


class PlanImage : public Image {
public:
	int pivotX, pivotY, theR;
	PlanImage();
	PlanImage(string filePath);

	PlanImage(Vector2u si);
};

double binpow(double x, int n);
Color interpolation(double x, double y, const PlanImage& image);
double test_distorce(const PlanImage& img, const Color& test_color);
unique_ptr<PlanImage> distorce(const PlanImage& inImage, double coef[NUMCOEF]);
unique_ptr<PlanImage> distorce_dirch(const PlanImage& inImage, double f, double k);
bool test_ends(const PlanImage& image, Color test_color);








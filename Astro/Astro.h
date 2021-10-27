#pragma once
#include <iostream>
#include <fstream>
#include <memory>
#include <limits>
#define _USE_MATH_DEFINES
#include <math.h>
#include "SFML/Graphics.hpp"

using namespace std;
using namespace sf;

#define NUMCOEF 3
#define THRESHOLD 30
#define LOWER_LIMIT 0.8
#define UPPER_LIMIT 1.3
#define	MIN_DIFF_ANGLE M_PI / 12

typedef Vector2<double> Vector2d;

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
	vector<vector<double>> precalc;
	int pivotX, pivotY, theR;
	PlanImage();
	PlanImage(string filePath);

	PlanImage(Vector2u si);
	void initNewR(double coef[NUMCOEF]);
};

void roundArr(double c[NUMCOEF]);
double binpow(double x, int n);
Color interpolation(double x, double y, const PlanImage& image);
double test_distorce(const PlanImage& img, const Color& test_color);
double fun(double r, double c[NUMCOEF]);
bool test_sign(double r_max, double c[NUMCOEF]);
unique_ptr<PlanImage> distorce(const PlanImage& inImage, double coef[NUMCOEF]);
unique_ptr<PlanImage> distorce_dirch(const PlanImage& inImage, double f, double k);







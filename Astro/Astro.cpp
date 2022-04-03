#include "Astro.h"

SmartImage::SmartImage() : Image(), pivotX(0), pivotY(0), theR(0) {}

SmartImage::SmartImage(string filePath) : Image() {
	if (!loadFromFile(filePath)) {
		throw runtime_error("Не удалось загрузить изображение из файла " + filePath);
	}
	else {
		pivotX = getSize().x / 2;
		pivotY = getSize().y / 2;
		theR = hypot(pivotX, pivotY);
	}
}

SmartImage::SmartImage(Vector2u si) : Image() {
	create(si.x, si.y);
	pivotX = si.x / 2;
	pivotY = si.y / 2;
	theR = hypot(pivotX, pivotY);
}

void SmartImage::initNewR(const double coef[NUMCOEF]) {
	++pivotX;
	++pivotY;
	precalc.resize(pivotX);
	precalc[0].resize(pivotY);
	precalc[0][0] = 0;
	for (int i = 1; i < pivotX; ++i) {
		precalc[i].resize(pivotY);
		precalc[i][0] = func(i, coef) / i;
		if (i < pivotY)
			precalc[0][i] = precalc[i][0];
	}
	for (int i = 1; i < pivotX; ++i) {
		for (int j = 1; j <= min(i, pivotY - 1); ++j) {
			precalc[i][j] = func(hypot(i, j), coef) / hypot(i, j);
			if (i < pivotY && j < pivotX)
				precalc[j][i] = precalc[i][j];
		}
	}
	
	--pivotX;
	--pivotY;
}


//////////////////////////////////////////////////////////////////////////////////////


NumColor::NumColor() : Color() {}

NumColor::NumColor(Color c) : Color(c) {}

NumColor::NumColor(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha)
	: Color(red, green, blue, alpha) {}

NumColor NumColor::operator*(const double& k) const {
	return NumColor(r * k, g * k, b * k, a);
}

// Returns absolut value!
int NumColor::operator-(const NumColor& other) const
{
	return abs(int(r) - other.r) + abs(int(g) - other.g) + abs(int(b) - other.b);
}

//-----------------------------------------------------------------------------

void roundArr(double c[NUMCOEF])
{
	for (int i = 0; i < NUMCOEF; ++i)
		c[i] = round(c[i] * 1e8) / 1e8;

}

double binpow(double x, int n) {
	if (n == 0)
		return 1;
	if (n % 2)
		return binpow(x, n - 1) * x;
	else
		return sqr(binpow(x, n / 2));
}

//-----------------------------------------------------------------------------

Color interpolation(double x, double y, const SmartImage* image) {
	NumColor pixel;
	double x1 = floor(x), x2 = ceil(x), y1 = floor(y), y2 = ceil(y);
	if (x1 == x2 && y1 == y2)
		return image->getPixel(x, y);
	if (x1 == x2) {
		pixel = NumColor(image->getPixel(x, y1)) * (y2 - y) + NumColor(image->getPixel(x, y2)) * (y - y1);
		return pixel;
	}
	if (y1 == y2) {
		pixel = NumColor(image->getPixel(x1, y)) * (x2 - x) + NumColor(image->getPixel(x2, y)) * (x - x1);
		return pixel;
	}

	NumColor r1 = NumColor(image->getPixel(x1, y1)) * (x2 - x) + NumColor(image->getPixel(x2, y1)) * (x - x1);
	NumColor r2 = NumColor(image->getPixel(x1, y2)) * (x2 - x) + NumColor(image->getPixel(x2, y2)) * (x - x1);
	pixel = r1 * ((y2 - y) / (y2 - y1)) + r2 * ((y - y1) / (y2 - y1));
	return pixel;
}

//-----------------------------------------------------------------------------

// Проверить, насколько правильно искривили; изображение передаётся по константной ссылке
double test_distorce(const SmartImage* img, const Color& test_color) {
	int sumx = 0, sumy = 0, sumx2 = 0, sumxy = 0, cnt = 0;
	vector<Vector2u> coords;
	for (int x = 0; x < img->getSize().x; x++) {
		for (int y = 0; y < img->getSize().y; y++) {
			if (NumColor(img->getPixel(x, y)) - NumColor(test_color) < THRESHOLD) {
				//if (cnt < 15)
					//cout << (int)img->getPixel(x, y).r << " " << (int)img->getPixel(x, y).g << " " << (int)img->getPixel(x, y).b << "\n";
				++cnt;
				sumx += x;
				sumx2 += x * x;
				sumxy += x * y;
				sumy += y;
				coords.emplace_back(x, y);
			}
		}
	}

	if (cnt < 5)
		return numeric_limits<double>::max();

	if (sumx * sumx / cnt == sumx2)
		return 0;

	double a = (double(sumxy) - double(sumx) * sumy / cnt) / (double(sumx2) - double(sumx) * sumx / cnt);
	double b = double(sumy - a * sumx) / cnt;
	double ans = 0;

	for (auto i : coords) {
		ans += sqr(a * i.x + b - i.y);
	}

	return ans / pow(cnt, 1.5);
}

//-----------------------------------------------------------------------------

double func(double r, const double c[NUMCOEF]) {
	return c[2] * binpow(r, 3) + c[1] * r * r + c[0] * r;
}

/*bool test_sign(double r_max, const double c[NUMCOEF])
{
	double r1, r2;
	if (c[0] < 0) // если значение производной в нуле отрицательно, нам не годится
		return false;
	if (c[0] == 0) { // проще рассмотреть этот случай сейчас, чтоб он потом не путался под ногами
		if (c[2] == 0) return c[1] > 0;
		r1 = -2. * c[1] / (3. * c[2]);
		if (r1 > 0 && r1 < r_max) return false;
		if (r1 <= 0) return c[2] > 0;
		else return c[2] < 0;
	}
	if (c[2] == 0) {
		if (c[1] == 0) return true;
		r1 = -c[0] / (2 * c[1]);
		return r1 <= 0 || r1 >= r_max;
	}

	if (fabs(c[0] * c[2]) < 10000. * DBL_EPSILON * c[1] * c[1]) {
		r1 = -2. * c[1] / (3. * c[2]);
		r2 = -c[0] / (2 * c[1]);
	}
	else {
		double D4 = c[1] * c[1] - 3.0 * c[0] * c[2];
		if (D4 < 0)
			return true;
		r1 = (-c[1] - sqrt(D4)) / (3 * c[2]);
		r2 = (-c[1] + sqrt(D4)) / (3 * c[2]);
	}
	if (r1 > r2)
		swap(r1, r2);
	if (r1 >= r_max || r2 < 0)
		return true;
	if (r1 > 0 || r2 < r_max)
		return false;
	return true;
}*/


/*double d_test_sign(double r_max, const double c[NUMCOEF])
{
	// 3c_2 * d^2 + 2*c_1 * d + c_0
	double x = -(c[1]) / (3 * c[2]);// -b / (2 * a)
	x = min(max(x, 0.), r_max);
	return x * (r_max - x);

}*/

double cont_test_sign(double r_max, const double c[NUMCOEF]) {
	double r1, r2;
	if (c[0] < 0) { // если значение производной в нуле отрицательно, нам не годится
		cout << "in test_sign c[0] " << c[0] << " < 0\n";
		return c[0];
	}
	if (c[2] == 0) {
		if (c[1] >= 0) return c[0];
		return 2 * c[1] * r_max + c[0];
	}
	if (c[2] > 0) {
		double x = -(c[1]) / (3 * c[2]); // -b / (2 * a)
		x = min(max(x, 0.), r_max);
		return 3 * c[2] * x * x + 2 * c[1] * x + c[0];
	}
	return min(c[0], 3 * c[2] * r_max * r_max + 2 * c[1] * r_max + c[0]);
	
}

//-----------------------------------------------------------------------------

SmartImage* distorce(const SmartImage* inImage, const double coef[NUMCOEF]) {
	Vector2u imgSize = inImage->getSize();

	double theta = M_PI / 4;

	SmartImage* outImage = new SmartImage(imgSize);	// Итоговое изображение.
	outImage->initNewR(coef);

	int xx, yy;
	double alpha, r, sourceX, sourceY;

	for (int x = 0; x < imgSize.x; x++) {
		for (int y = 0; y < imgSize.y; y++) {
			xx = x - inImage->pivotX;
			yy = y - inImage->pivotY;

			sourceX = inImage->pivotX + outImage->precalc[abs(xx)][abs(yy)] * xx;
			sourceY = inImage->pivotY + outImage->precalc[abs(xx)][abs(yy)] * yy;

			if (sourceX < 0 || sourceX >= imgSize.x - 1 || sourceY < 0 || sourceY >= imgSize.y - 1)
				continue;

			outImage->setPixel(x, y, interpolation(sourceX, sourceY, inImage));
		}
	}

	return outImage;
}

SmartImage* distorce_dirch(const SmartImage* inImage, double f, double k) {
	Vector2u imgSize = inImage->getSize();

	double theta = M_PI / 4;

	if (f == 0.0) {
		if (k == 0.0) f = inImage->theR / theta;
		else if (k > 0.0) f = inImage->theR * k / tan(k * theta);
		else { f = inImage->theR * k / sin(k * theta); if (k < -1) f *= fabs(k); }
	}
	SmartImage* outImage = new SmartImage(imgSize);	// Итоговое изображение.

	int xx, yy;
	double alpha, r, dist, sourceX, sourceY, phi;

	for (int x = 0; x < imgSize.x; x++) {
		for (int y = 0; y < imgSize.y; y++) {
			xx = x - inImage->pivotX;
			yy = y - inImage->pivotY;
			dist = sqrt(xx * xx + yy * yy);
			alpha = atan2((double)yy, (double)xx);

			phi = dist / f;
			if (k == 0.0) r = f * phi;
			else if (k < 0.0) r = f * sin(phi * k) / k;
			else r = f * tan(phi * k) / k;

			double sourceX = inImage->pivotX + r * cos(alpha);
			double sourceY = inImage->pivotY + r * sin(alpha);

			if (sourceX < 0 || sourceX >= imgSize.x - 1 || sourceY < 0 || sourceY >= imgSize.y - 1)
				continue;

			outImage->setPixel(x, y, interpolation(sourceX, sourceY, inImage));
		}
	}
	return outImage;
}
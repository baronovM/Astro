#include "Astro.h"

PlanImage::PlanImage() : Image(), pivotX(0), pivotY(0), theR(0) {}

PlanImage::PlanImage(string filePath) : Image() {
	if (!loadFromFile(filePath)) {
		//cerr << "НЕ УДАЛОСЬ СЧИТАТЬ ИЗОБРАЖЕНИЕ " << filePath << endl;
		throw 1;
	}
	else {
		pivotX = getSize().x / 2;
		pivotY = getSize().y / 2;
		theR = min(pivotX, pivotY);
	}
}

PlanImage::PlanImage(Vector2u si) : Image() {
	create(si.x, si.y);
	pivotX = si.x / 2;
	pivotY = si.y / 2;
	theR = min(pivotX, pivotY);
}


//////////////////////////////////////////////////////////////////////////////////////


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

////////////////////////////////////////////////////////////////////////////////////

double binpow(double x, int n) {
	if (n == 0)
		return x;
	if (n % 2)
		return binpow(x, n - 1) * x;
	else
		return sqr(binpow(x, n / 2));
}

Color interpolation(double x, double y, const PlanImage& image) {
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

// Проверить, насколько правильно искривили; изображение передаётся по константной ссылке
double test_distorce(const PlanImage& img, const Color& test_color) {
	int sumx(0), sumy(0), sumx2(0), sumxy(0), cnt = 0, r, g, b;
	vector<Vector2u> coords;
	for (int x = 0; x < img.getSize().x; x++) {
		for (int y = 0; y < img.getSize().y; y++) {
			r = abs(img.getPixel(x, y).r - test_color.r);
			g = abs(img.getPixel(x, y).g - test_color.g);
			b = abs(img.getPixel(x, y).b - test_color.b);
			if (r + g + b < THRESHOLD) {
				++cnt;
				sumx += x;
				sumx2 += x * x;
				sumxy += x * y;
				sumy += y;
				coords.emplace_back(x, y);
			}
		}
	}

	if (cnt < 3) {
		//cerr << "ТОЧЕК МЕНЬШЕ 3\n";
		return 1e20;
	}

	//sumx /= cnt; sumy /= cnt; sumx2 /= cnt; sumxy /= cnt;

	if (sumx * sumx / cnt == sumx2) {
		if (sumxy == sumx * sumy / cnt)
			return 0;
		return 1e8;
	}

	double a = (double(sumxy) - double(sumx) * sumy / cnt) / (double(sumx2) - double(sumx) * sumx / cnt);
	double b_ = double(sumy - a * sumx) / cnt;
	double ans = 0;

	//printf("\na: %lf, b: %lf, cnt: %d\n", a, b_, cnt);
	for (auto i : coords) {
		ans += sqr(a * i.x + b_ - i.y);
	}

	return ans / cnt;
}

unique_ptr<PlanImage> distorce(const PlanImage& inImage, double coef[NUMCOEF]) {

	Vector2u imgSize = inImage.getSize();

	double theta = M_PI / 4;

	unique_ptr<PlanImage> outImage = make_unique<PlanImage>(imgSize);	// Итоговое изображение.

	int xx, yy;
	double alpha, r, dist, sourceX, sourceY;

	for (int x = 0; x < imgSize.x; x++) {
		for (int y = 0; y < imgSize.y; y++) {
			xx = x - inImage.pivotX;
			yy = y - inImage.pivotY;
			dist = sqrt(xx * xx + yy * yy);
			alpha = atan2((double)yy, (double)xx);

			r = coef[0] * binpow(dist, 3) + coef[1] * dist * dist + coef[2] * dist;

			sourceX = inImage.pivotX + r * cos(alpha);
			sourceY = inImage.pivotY + r * sin(alpha);

			if (sourceX < 0 || sourceX >= imgSize.x - 1 || sourceY < 0 || sourceY >= imgSize.y - 1)
				continue;

			outImage->setPixel(x, y, interpolation(sourceX, sourceY, inImage));
		}
	}

	return outImage;
}

unique_ptr<PlanImage> distorce_dirch(const PlanImage& inImage, double f, double k) {

	Vector2u imgSize = inImage.getSize();

	double theta = M_PI / 4;

	if (f == 0.0) {
		if (k == 0.0) f = inImage.theR / theta;
		else if (k > 0.0) f = inImage.theR * k / tan(k * theta);
		else { f = inImage.theR * k / sin(k * theta); if (k < -1) f *= fabs(k); }
	}
	unique_ptr<PlanImage> outImage = make_unique<PlanImage>(imgSize);	// Итоговое изображение.

	int xx, yy;
	double alpha, r, dist, sourceX, sourceY;

	for (int x = 0; x < imgSize.x; x++) {
		for (int y = 0; y < imgSize.y; y++) {
			int xx = x - inImage.pivotX;
			int yy = y - inImage.pivotY;
			double alpha, r, dist = sqrt(xx * xx + yy * yy);
			alpha = atan2((double)yy, (double)xx);

			double phi = dist / f;
			if (k == 0.0) r = f * phi;
			else if (k < 0.0) r = f * sin(phi * k) / k;
			else r = f * tan(phi * k) / k;

			double sourceX = inImage.pivotX + r * cos(alpha);
			double sourceY = inImage.pivotY + r * sin(alpha);

			if (sourceX < 0 || sourceX >= imgSize.x - 1 || sourceY < 0 || sourceY >= imgSize.y - 1)
				continue;

			outImage->setPixel(x, y, interpolation(sourceX, sourceY, inImage));
		}
	}
	return outImage;
}
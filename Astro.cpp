#include "Astro.h"

PlanImage::PlanImage() : Image(), pivotX(0), pivotY(0), theR(0) {}

PlanImage::PlanImage(string filePath) : Image() {
	if (!loadFromFile(filePath)) {
		throw "Не удалось загрузить изображение из файла " + filePath;
	}
	else {
		pivotX = getSize().x / 2;
		pivotY = getSize().y / 2;
		theR = sqrt(pivotX * pivotX + pivotY * pivotX);
	}
}

PlanImage::PlanImage(Vector2u si) : Image() {
	create(si.x, si.y);
	pivotX = si.x / 2;
	pivotY = si.y / 2;
	theR = sqrt(pivotX * pivotX + pivotY * pivotX);
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
Uint8 NumColor::operator-(const NumColor& other) const
{
	return abs(r - other.r) + abs(g - other.g) + abs(b - other.b);
}

//-----------------------------------------------------------------------------

double binpow(double x, int n) {
	if (n == 0)
		return 1;
	if (n % 2)
		return binpow(x, n - 1) * x;
	else
		return sqr(binpow(x, n / 2));
}

//-----------------------------------------------------------------------------

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

//-----------------------------------------------------------------------------

// Проверить, насколько правильно искривили; изображение передаётся по константной ссылке
double test_distorce(const PlanImage& img, const Color& test_color) {
	int sumx = 0, sumy = 0, sumx2 = 0, sumxy = 0, cnt = 0;
	vector<Vector2u> coords;
	for (int x = 0; x < img.getSize().x; x++) {
		for (int y = 0; y < img.getSize().y; y++) {
			if (NumColor(img.getPixel(x, y)) - NumColor(test_color) < THRESHOLD) {
				++cnt;
				sumx += x;
				sumx2 += x * x;
				sumxy += x * y;
				sumy += y;
				coords.emplace_back(x, y);
			}
		}
	}

	if (cnt < 3)
		return numeric_limits<double>::max();

	if (sumx * sumx / cnt == sumx2) {
		if (sumxy == sumx * sumy / cnt)
			return 0;
		return numeric_limits<double>::max();
	}

	double a = (double(sumxy) - double(sumx) * sumy / cnt) / (double(sumx2) - double(sumx) * sumx / cnt);
	double b_ = double(sumy - a * sumx) / cnt;
	double ans = 0;

	for (auto i : coords) {
		ans += sqr(a * i.x + b_ - i.y);
	}

	return ans / cnt;
}

//-----------------------------------------------------------------------------

double fun(double r, double c[NUMCOEF]) {
	return c[2] * binpow(r, 3) + c[1] * r * r + c[0] * r;
}

bool test_sign(double r_max, double c[NUMCOEF])
{
	double r1,r2;
	if(c[0]<0) // если значение производной в нуле отрицательно, намъ не годится
		return false;
	if(c[0]==0){ // проще рассмотреть этотъ случай сейчасъ, чтобъ онъ потомъ не путался подъ ногами
	 if(c[2]==0) return c[1]>0;	 //если прямая проходитъ черезъ 0, всё определяется знакомъ c[1] - пойдётъ прямая вверхъ или внизъ
	 r1=-2.*c[1]/(3.*c[2]);
	 if(r1>0 && r1<r_max) return false;
	 if(r1<=0) return c[2]>0;
	 else return c[2]<0;
	}
 	if (c[2] == 0) {
	 if(c[1] == 0) return true;	 //если прямая проходитъ черезъ 0, всё определяется знакомъ c[1] - пойдётъ прямая вверхъ или внизъ	 
	 r1 = -c[0] / (2 * c[1]); //  въ этомъ случа? есть только одинъ корень
	 return r1 <= 0 || r1 >= r_max;
	}	
	
	if(fabs(c[0]*c[2])<10000.*DBL_EPSILON*c[1]*c[1]){
     r1=-2.*c[1]/(3.*c[2]);
     r2=-c[0] / (2 * c[1]); 
	}
	else{ 
 	 double D4 = c[1] * c[1] - 3.0 * c[0] * c[2];
	 if(D4<0)
  		return true; // мы уже пров?рили что с[0]>=0, т. е. в нул? значение положительно. Если дискриминант отрицателен, тогда функция везде положительна
	 r1 = (-c[1] - sqrt(D4)) / (3 * c[2]); // также, мы пров?рили что с2 не слишкомъ мало, такъ что можно на него д?лить.
	 r2 = (-c[1] + sqrt(D4)) / (3 * c[2]);
	}	
	if (r1 > r2)
 	 swap(r1, r2);
	if (r1 >= r_max || r2 < 0)
	 return true;
	if (r1 > 0 || r2 < r_max)
	 return false;
	return true;
}

//-----------------------------------------------------------------------------

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

			r = fun(dist, coef);
			//car[0] * binpow(inImage.theR, 3) + car[1] * inImage.theR * inImage.theR + car[2] * inImage.theR

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
	double alpha, r, dist, sourceX, sourceY, phi;

	for (int x = 0; x < imgSize.x; x++) {
		for (int y = 0; y < imgSize.y; y++) {
			xx = x - inImage.pivotX;
			yy = y - inImage.pivotY;
			dist = sqrt(xx * xx + yy * yy);
			alpha = atan2((double)yy, (double)xx);

			phi = dist / f;
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

bool test_ends(const PlanImage& image, Color test_color){
	
	int xx, yy;
	bool bad = false;
	double alpha;
	for (int x = 0; x < image.getSize().x; ++x){
		for (int y = 0; y < image.getSize().y; ++y) {
			xx = x - image.pivotX;
			yy = y - image.pivotY;
			if (NumColor(image.getPixel(x, y)) - NumColor(test_color) < THRESHOLD) {
				if (!bad) {
					alpha = atan2((double)yy, (double)xx);
					bad = true;
				}
				else if (bad && abs(alpha - atan2((double)yy, (double)xx)) > MIN_DIFF_ANGLE) {
					return true;
				}
			}
		}
	}

	return false;
}

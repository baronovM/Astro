#include "Astro.h"

int main(int argc, char** argstr) {
	string imagePath, outImagePath;
	if (argc == 3) {
		imagePath = argstr[1];
		outImagePath = argstr[2];
	}
	else
		cout << "¬ведите названи€ входного и выходного изображений\n";

	SmartImage* inImage = new SmartImage(imagePath);
	Color test_color(0, 255, 0);

	double test_r;

	double coef[NUMCOEF];	// coefficient array
	double best[NUMCOEF];
	double mn = 1e20, cur;
	for (coef[0] = 0.; coef[0] < 5.0; coef[0] += 0.05) {
		for (coef[1] = -2.5; coef[1] < 2.5; coef[1] += 0.025) {
			for (coef[2] = -0.05; coef[2] < 0.05; coef[2] += 0.0005) {
				roundArr(coef);
				test_r = func(inImage->theR, coef);
				if (LOWER_LIMIT * inImage->theR < test_r && test_r < UPPER_LIMIT * inImage->theR
					&& cont_test_sign(inImage->theR, coef) > 0) {
					ostringstream temp;
					for (int i = 0; i < NUMCOEF; ++i)
						temp << "__" << coef[i];
					SmartImage* img;
					if (!filesystem::exists("out/" + imagePath + temp.str() + ".png")) {
						img = distorce(inImage, coef);
						img->saveToFile("out/" + imagePath + temp.str() + ".png");
					}
					else {
						img = new SmartImage("out/" + imagePath + temp.str() + ".png");
					}
					cur = test_distorce(img, test_color);
					if (cur < mn) {
						memcpy(best, coef, NUMCOEF * sizeof(double));
						mn = cur;
					}
					delete img;
				}
			}
		}
	}
	ofstream fout("coef.txt");
	for (auto i : best) {
		fout << i << " ";
	}
	fout.close();

	SmartImage* result = distorce(inImage, best);
	result->saveToFile(outImagePath);
	delete inImage;
	delete result;

	return 0;
}

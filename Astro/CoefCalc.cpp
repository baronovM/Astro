#include "Astro.h"
#include <sstream>

int main(int argc, char** argstr) {
	string imagePath, outImagePath;
	if (argc == 3) {
		imagePath = argstr[1];
		outImagePath = argstr[2];
	}
	else
		cout << "Названия входного и выходного изображений - {Название входного, без пробелов и с расширением} {Название выходного}:\n";

	PlanImage* inImage = new PlanImage(imagePath);
	Color test_color(255, 0, 255);

	double test_r;

	double coef[NUMCOEF];	// coefficient array
	double best[NUMCOEF];
	double mn = 1e20, cur;
	for (coef[0] = 0.; coef[0] < 4.0; coef[0] += 0.05) {
		for (coef[1] = -2.5; coef[1] < 2.5; coef[1] += 0.025) {
			for (coef[2] = -0.05; coef[2] < 0.05; coef[2] += 0.0005) {
				roundArr(coef);
				test_r = fun(inImage->theR, coef);
				if (LOWER_LIMIT * inImage->theR < test_r && test_r < UPPER_LIMIT * inImage->theR
				&& test_sign(inImage->theR, coef)) {
					ostringstream temp;
					for (int i = 0; i < NUMCOEF; ++i)
						temp << "__" << coef[i];
					PlanImage* img;
					if (!filesystem::exists("out/" + imagePath + temp.str() + ".png")) {
						img = distorce(inImage, coef);
						img->saveToFile("out/" + imagePath + temp.str() + ".png");
					}
					else {
						img = new PlanImage("out/" + imagePath + temp.str() + ".png");
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
	delete inImage;
	ofstream fout("coef.txt");
	for (auto i : best) {
		fout << i << " ";
	}
	fout.close();

	PlanImage* result = distorce(inImage, best);
	result->saveToFile(outImagePath);
	delete result;

	return 0;
}

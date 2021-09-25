#include "Astro.h"
#include <filesystem>
#include <sstream>

int main(int argc, char** argstr) {
	setlocale(LC_ALL, "RUS");

	string imagePath, outImagePath;
	if (argc == 3) {
		imagePath = argstr[1];
		outImagePath = argstr[2];
	}
	else
		cout << "Названия входного и выходного изображений - {Название входного, без пробелов и с расширением} {Название выходного}:\n";

	PlanImage inImage(imagePath);
	Color test_color(255, 0, 255, 255);
	Color test_color_2(0, 255, 0, 255);

	double test_r;

	double car[NUMCOEF];
	double best[NUMCOEF];
	double mn = 1e20, cur;
	for (car[0] = -20.; car[0] < 20.; car[0] += 0.1) {
		for (car[1] = -20.; car[1] < 20.; car[1] += 0.1) {
			for (car[2] = -20.; car[2] < 20.; car[2] += 0.1) {
				test_r = car[0] * binpow(inImage.theR, 3) + car[1] * inImage.theR * inImage.theR + car[2] * inImage.theR;
				if (LOWER_LIMIT * inImage.theR < test_r && test_r < UPPER_LIMIT * inImage.theR) {
					ostringstream temp;
					for (int i = 0; i < NUMCOEF; ++i)
						temp << "__" << car[i];
					unique_ptr<PlanImage> img;
					if (!filesystem::exists("out/" + imagePath + temp.str() + ".png")) {
						img = distorce(inImage, car);
						img->saveToFile("out/" + imagePath + temp.str() + ".png");
						cur = test_distorce(*img, test_color);
					}
					else {
						img = make_unique<PlanImage>("out/" + imagePath + temp.str() + ".png");
						cur = test_distorce(*img, test_color);
					}
					if (cur < mn && test_ends(*img, test_color_2)) {
						memcpy(best, car, NUMCOEF * sizeof(double));
						mn = cur;
					}
				}
			}
		}
	}
	ofstream fout("coef.txt");
	for (auto i : best) {
		fout << i << " ";
	}
	fout.close();

	unique_ptr<PlanImage> result = distorce(inImage, best);
	result->saveToFile(outImagePath);
	if (test_ends(*result, test_color_2))
		cout << "YEEES";
	else
		cout << "NO";

	return 0;
}

#include "Astro.h"
#include <filesystem>

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

	//double test_temp;

	double car[NUMCOEF];
	double best[NUMCOEF];
	double mn = 1e20, cur;
	for (car[0] = -1.; car[0] < 1.; car[0] += 0.1) {
		for (car[1] = -1.; car[1] < 1.; car[1] += 0.1) {
			for (car[2] = -1.; car[2] < 1.; car[2] += 0.1) {
				//test_temp = f * (car[0] * binpow(M_PI / 2, 6) + car[1] * binpow(M_PI / 2, 4) + car[2] * binpow(M_PI / 2, 2));
				//if (0.2 * inImage.theR < test_temp && inImage.theR < 2 * inImage.theR) {
				string temp = "";
				for (int i = 0; i < NUMCOEF; ++i)
					temp += "_" + to_string(car[i]);
				unique_ptr<PlanImage> img;
				if (!filesystem::exists("out/" + imagePath + "_" + temp + ".png")) {
					img = distorce(inImage, car);
					img->saveToFile("out/" + imagePath + "_" + temp + ".png");
				}
				else {
					img->loadFromFile("out/" + imagePath + "_" + temp + ".png");
				}
				cur = test_distorce(*img, test_color);
				if (cur < mn) {
					memcpy(best, car, NUMCOEF * sizeof(double));
					mn = cur;
				}
				//}
				//cout << "\n";
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

	return 0;
}

#include "Astro.h"
#include <filesystem>

int main(int argc, char** argstr) {
	setlocale(LC_ALL, "RUS");
	double k;	// Интенсивность коррекции.
	double f;	// Фокусное расстояние, по умолчанию - "чтобы экватор сферы был вписан в наименьший размер изображения".


	string imagePath, outImagePath;
	if (argc == 1) {
		cout << "Введите коэффициент k в процентах, названия входного и выходного изображений - {k} {Название входного, без пробелов и с расширением} {Название выходного}:\n";
		cin >> k >> imagePath >> outImagePath;
	}
	else {
		k = atoi(argstr[1]);
		imagePath = argstr[2];
		outImagePath = argstr[3];
	}


	k /= 100.;

	f = 0.0;

	PlanImage inImage(imagePath);
	Color test_color(255, 0, 255, 255);

	//double test_temp;

	double car[NUMCOEF];
	double best[NUMCOEF];
	double mn = 1e20, cur;
	for (car[0] = -10.; car[0] < 10.; car[0] += 1.) {
		for (car[1] = -10.; car[1] < 10.; car[1] += 1.) {
			for (car[2] = -10.; car[2] < 10.; car[2] += 1.) {
				//test_temp = f * (car[0] * binpow(M_PI / 2, 6) + car[1] * binpow(M_PI / 2, 4) + car[2] * binpow(M_PI / 2, 2));
				//if (0.2 * inImage.theR < test_temp && inImage.theR < 2 * inImage.theR) {
				string temp = "";
				for (int i = 0; i < NUMCOEF; ++i)
					temp += "_" + to_string(car[i]);
				PlanImage* img = new PlanImage;
				if (!filesystem::exists("out/" + imagePath + "_" + temp + ".png")) {
					img = distorce(inImage, f, k, car);
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

	distorce(inImage, f, k, best)->saveToFile(outImagePath);

	system("pause");
	return 0;
}

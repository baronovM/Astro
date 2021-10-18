#include "Astro.h"

int main(int argc, char** argstr) {
	setlocale(LC_ALL, "RUS");
	double k;	// Интенсивность коррекции.
	double f;	// Фокусное расстояние, по умолчанию - "чтобы экватор сферы был вписан в наименьший размер изображения".

	string imagePath, outImagePath;
	if (argc == 1) {
		cout << "Введите коэффициент k в процентах, названия входного и выходного изображений - {k} {Название входного, без пробелов и с расширением} {Название выходного}:\n";
		cin >> k >> imagePath >> outImagePath;
	}
	else if (argc >= 4) {
		k = atoi(argstr[1]);
		imagePath = argstr[2];
		outImagePath = argstr[3];
	}
	else {
		cout << "dirc.cpp k, inImage, outImage";
		return 0;
	}

	k = k / 100.;
	f = 0.0;

	PlanImage inImage(imagePath);

	unique_ptr<PlanImage> outImage = distorce_dirch(inImage, f, k);
	outImage->saveToFile(outImagePath);

	return 0;
}
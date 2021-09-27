#include "Astro.h"

int main(int argc, char** argv) {
	double coef[NUMCOEF];
	string inImagePath, outImagePath;
	if (argc != NUMCOEF + 3) {
		printf("Используйте distorce.exe {название входного изображения} {название выходного} {coef0} {coef1} {coef2}\n");
		cin >> inImagePath >> outImagePath;
		for (int i = 0; i < NUMCOEF; ++i)
			cin >> coef[i];
	}
	else {
		for (int i = 0; i < NUMCOEF; ++i)
			coef[i] = atof(argv[i + 3]);
		inImagePath = argv[1];
		outImagePath = argv[2];
	}
	PlanImage inImage(inImagePath);
	unique_ptr<PlanImage> outImagePtr = distorce(inImage, coef);
	outImagePtr->saveToFile(outImagePath);

	return 0;
}
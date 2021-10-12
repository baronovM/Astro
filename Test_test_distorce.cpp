#include "Astro.h"

int main(int argc, char** argstr) {
	setlocale(LC_ALL, "RUS");
	string imagePath, outImagePath;
	if (argc == 1) {
		cout << "������� �������� �������� �����������:\n";
		cin >> imagePath;
	}
	else {
		imagePath = argstr[1];
	}
	PlanImage inImage(imagePath);
	cout << test_distorce(inImage, Color(255, 0, 255, 255));


	return 0;
}
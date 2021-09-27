#include "Astro.h"

int main(int argc, char** argstr) {
	setlocale(LC_ALL, "RUS");
	double k;	// ������������� ���������.
	double f;	// �������� ����������, �� ��������� - "����� ������� ����� ��� ������ � ���������� ������ �����������".

	string imagePath, outImagePath;
	if (argc == 1) {
		cout << "������� ����������� k � ���������, �������� �������� � ��������� ����������� - {k} {�������� ��������, ��� �������� � � �����������} {�������� ���������}:\n";
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
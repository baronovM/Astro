#include "Astro.h"

int main(int argc, char** argstr) {
	setlocale(LC_ALL, "RUS");
	double coef[NUMCOEF];
	if (argc == NUMCOEF + 1) {
		for (int i = 0; i < NUMCOEF; ++i)
		{
			coef[i] = atof(argstr[i + 1]);
		}
	}
	else {
		cout << "Коэффициенты введи\n";
		cin >> coef[0] >> coef[1] >> coef[2];
	}
	//printf("%.2f %.2f %.2f\n", coef[0], coef[1], coef[2]);
	if (test_sign(35, coef))
		cout << "YEEEEES";
	else
		cout << "no";
	return 0;
}

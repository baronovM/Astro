//������ ������������� ������� atof
#include <iostream>
#include <cstdlib>
#include <cmath>

int main()
{
    setlocale(LC_ALL, "RUS");
    double pi = 3.1415926535;                  // ����� ��
    char input[256];
    std::cout << "������� ���� � ��������: ";
    std::cin >> input;

    double agl = atof(input);                // ����������� ������ � ���� ������ float

    std::cout << "����� ���� "
        << agl << " �������� = "
        << sin(agl * pi / 180) << "n"; // ��������� ����� ����, ������������ � �������
    return 0;
}
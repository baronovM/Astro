//пример использования функции atof
#include <iostream>
#include <cstdlib>
#include <cmath>

int main()
{
    setlocale(LC_ALL, "RUS");
    double pi = 3.1415926535;                  // число ПИ
    char input[256];
    std::cout << "Введите угол в градусах: ";
    std::cin >> input;

    double agl = atof(input);                // преобразуем строку к типу данных float

    std::cout << "Синус угла "
        << agl << " градусов = "
        << sin(agl * pi / 180) << "n"; // вычисляем синус угла, переведённого в радианы
    return 0;
}
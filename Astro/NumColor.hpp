#ifndef NumColor
#define NumColor

#include <SFML/Graphics.hpp>

using namespace std;

class NumColor {
	sf::Uint8 r, g, b, a;
public:
	NumColor(sf::Uint8 r1, sf::Uint8 g1, sf::Uint8 b1, sf::Uint8 a1) {
		r = r1;
		g = g1;
		b = b1;
		a = a1;
	}
	NumColor operator*(const int& k) const {
		return NumColor(r * k, g * k, b * k, a);
};

#endif
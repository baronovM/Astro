#include "NumColor.hpp"


NumColor::NumColor() {
	r = 0; g = 0; b = 0; a = 0;
}

NumColor::NumColor(Color c) {
	r = c.r;
	g = c.g;
	b = c.b;
	a = c.a;
}

NumColor::NumColor(Uint8 r1, Uint8 g1, Uint8 b1, Uint8 a1) {
	r = r1;
	g = g1;
	b = b1;
	a = a1;
}

NumColor NumColor::operator*(const double& k) const {
	return NumColor(r * k, g * k, b * k, a);
}
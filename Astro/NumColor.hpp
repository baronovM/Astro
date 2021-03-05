#ifndef NumCol
#define NumCol

#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

class NumColor : public Color {
public:
	NumColor();

	NumColor(Color c);

	NumColor(Uint8 r1, Uint8 g1, Uint8 b1, Uint8 a1);
	NumColor operator*(const double& k) const;
};

#endif //!NumCol
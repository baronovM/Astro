#include <SFML/Graphics.hpp>

using namespace std;
using namespace sf;

int main()
{
	Image im;
	im.loadFromFile("image.jpg");
	im.flipHorizontally();
	im.saveToFile("new_image.jpg");
	return 0;
}
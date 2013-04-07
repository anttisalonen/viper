#include <iostream>
#include <stdexcept>

#include "Game.h"


int main(int argc, char** argv)
{
	try {
		Game game;
		game.go();
	} catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}

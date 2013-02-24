#include <iostream>
#include <stdexcept>

#include "App.h"


int main(int argc, char** argv)
{
	try {
		App app;
		app.go();
	} catch (std::exception& e) {
		std::cerr << "Exception: " << e.what() << std::endl;
		return 1;
	}
	return 0;
}

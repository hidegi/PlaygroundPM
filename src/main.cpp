#include <SP/base/application.h>
#include "PM/pacman.h"

int main(int argc, char** argv)
{
	sp::WindowConfig config;
	config.width = 16 * 21;
	config.height = 16 * 27;
	config.resizable = false;
	sp::Application app(config);
	return app.run<pm::Pacman>();
}
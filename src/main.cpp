#include <SP/base/surface.h>

int main(int argc, char** argv)
{
	sp::Surface surface;
	surface.create();
	while(surface.isRunning())
	{
		sp::pollEvents();
		surface.swapBuffers();
	}
	;
	return 0;
}
#include <sp/sp.h>
#include <pacman/pacman.h>
#include <sp/arb/levler.h>
#include <sp/arb/levels.h>
#include <sp/exsp/listener_sprb.h>
#include <sp/exsp/event_bus.h>


int main()
{
    PacMan pacman(21 * 16, 30 * 16);
    pacman.init();
    pacman.terminate();
    return 0;
}

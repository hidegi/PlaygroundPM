#ifndef DIRECTIONS_H_INCLUDED
#define DIRECTIONS_H_INCLUDED
#include <sp/ecs/machine.h>

enum Direction : int
{
    DIR_LEFT = 0,
    DIR_RIGHT,
    DIR_UP,
    DIR_DOWN,
    VULNERABLE,
    COOLDOWN
};

struct PowerEvent
{
};

struct WinEvent
{
};

struct CollisionEvent
{
    sp::entity_type target;
};

struct PointEvent
{
};

#endif // DIRECTIONS_H_INCLUDED

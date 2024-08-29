#ifndef POSITION_COMPONENT_H_INCLUDED
#define POSITION_COMPONENT_H_INCLUDED
#include <sp/math/vec.h>

struct PositionComponent
{
    sp::vec2f   last_position;
    sp::vec2f   next_position;

    sp::vec2f   origin;
    sp::vec2f   size;

    float       speed;
};


#endif // POSITION_COMPONENT_H_INCLUDED

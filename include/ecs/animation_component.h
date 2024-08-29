#ifndef ANIMATION_COMPONENT_H_INCLUDED
#define ANIMATION_COMPONENT_H_INCLUDED
#include <sp/math/vec.h>
#include <unordered_map>
#include <vector>
struct AnimationComponent
{
    float   frame_time;
    bool    loop;
    float   elapsed_time;
    int     current_frame;
    int     state;
    bool    playing;

    std::unordered_map<int, std::vector<sp::recti>>      frames;
};


#endif // ANIMATION_COMPONENT_H_INCLUDED

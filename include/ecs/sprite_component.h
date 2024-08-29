#ifndef SPRITECOMPONENT_H_INCLUDED
#define SPRITECOMPONENT_H_INCLUDED
#include <sp/gxsp/sprite.h>
struct SpriteComponent
{
    std::shared_ptr<sp::Sprite>   sprite;
};

#endif // SPRITECOMPONENT_H_INCLUDED

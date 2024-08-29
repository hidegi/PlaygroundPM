#ifndef ANIMATION_SYSTEM_H
#define ANIMATION_SYSTEM_H
#include <sp/ecs/sys/system.h>

class AnimationSystem : public sp::System
{
    public:
        typedef std::shared_ptr<AnimationSystem>            Ptr;
        typedef std::shared_ptr<const AnimationSystem>      ConstPtr;

        AnimationSystem(std::shared_ptr<sp::Controller> controller);
       ~AnimationSystem();

        virtual void    update(double dt) override;
};


#endif // ANIMATION_SYSTEM_H

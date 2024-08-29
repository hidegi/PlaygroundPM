#ifndef MOVEMENT_SYSTEM_H
#define MOVEMENT_SYSTEM_H
#include <sp/ecs/sys/system.h>

class MovementSystem : public sp::System
{
    public:
        typedef std::shared_ptr<MovementSystem>         Ptr;
        typedef std::shared_ptr<const MovementSystem>   ConstPtr;

        MovementSystem(std::shared_ptr<sp::Controller> controller);
        virtual void update(double dt) override;

};
#endif // MOVEMENT_SYSTEM_H

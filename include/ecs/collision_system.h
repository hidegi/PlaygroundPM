#ifndef COLLISIONSYSTEM_H
#define COLLISIONSYSTEM_H
#include <sp/ecs/machine.h>
#include <sp/ecs/sys/system.h>

//observes collision for one entity..
class CollisionSystem : public sp::System
{
    public:
        typedef std::shared_ptr<CollisionSystem>            Ptr;
        typedef std::shared_ptr<const CollisionSystem>      ConstPtr;

        CollisionSystem(std::shared_ptr<sp::Controller> controller);
       ~CollisionSystem();

       void setControlEntity(sp::entity_type entity);

       virtual void update(double dt) override;

    private:
        sp::entity_type   m_observable;
};


#endif // COLLISIONSYSTEM_H

#include <sp/sp_controller.h>
#include <pacman/ecs/collision_system.h>
#include <pacman/ecs/position_component.h>
#include <pacman/directions.h>

CollisionSystem::CollisionSystem(std::shared_ptr<sp::Controller> controller) :
    sp::System(controller),
    m_observable{0}
{
    m_observer->listen<sp::SysEventUpdate>([this](const sp::SysEventUpdate& event)
    {
        update(event.data.data);
    });
}

CollisionSystem::~CollisionSystem()
{
}

void CollisionSystem::setControlEntity(sp::entity_type entity)
{
    m_observable = entity;
}
void CollisionSystem::update(double dt)
{
    auto& comp = m_controller->getComponent<PositionComponent>(m_observable);
    //if(!comp) return;

    sp::rectf observer_bounds = sp::rectf{comp.last_position - comp.origin, comp.size};
    for(auto& delegate : m_controller->iterate<PositionComponent>())
    {
        if(delegate.first == m_observable)
            continue;

        sp::entity_type         entity    = delegate.first;
        PositionComponent& position = delegate.second;

        sp::rectf target_bounds = sp::rectf{position.last_position - position.origin, position.size};
        if(observer_bounds.intersects(target_bounds))
        {
            //printf("COLLISION!!\n");
            m_controller->getSubject()->dispatch(CollisionEvent{entity});
        }
    }
}

#include <sp/ecs/machine.h>
#include <sp/sp_controller.h>
#include <pacman/ecs/movement_system.h>
#include <pacman/ecs/position_component.h>
#include <pacman/ecs/sprite_component.h>

MovementSystem::MovementSystem(std::shared_ptr<sp::Controller> controller) :
    sp::System(controller)
{
    m_observer->listen<sp::SysEventUpdate>([this](const sp::SysEventUpdate& event)
    {
        update(event.data.data);
    });
}


void MovementSystem::update(double dt)
{
    for(auto& delegate : m_controller->iterate<PositionComponent>())
    {
        sp::entity_type     entity   = delegate.first;
        PositionComponent&  position = delegate.second;

        auto& sprite_comp = m_controller->getComponent<SpriteComponent>(entity);

        float speed           = position.speed;

        if(position.last_position != position.next_position)
        {
            sp::vec2f move_vector               = position.next_position - position.last_position;
            sp::vec2f move_vector_normalized    = move_vector.normalized();
            sp::vec2f movement = move_vector_normalized * speed * dt;
            if(move_vector.length() < movement.length())
            {
                position.last_position = position.next_position;
            }
            else
            {
                position.last_position += movement;
            }

            sprite_comp.sprite->setPosition(position.last_position);
        }
    }
}


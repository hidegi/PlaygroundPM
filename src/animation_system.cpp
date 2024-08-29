#include <sp/ecs/machine.h>
#include <sp/sp_controller.h>
#include <pacman/ecs/animation_system.h>
#include <pacman/ecs/animation_component.h>
#include <pacman/ecs/sprite_component.h>

AnimationSystem::AnimationSystem(std::shared_ptr<sp::Controller> controller) :
    sp::System(controller)
{
    m_observer->listen<sp::SysEventUpdate>([this](const sp::SysEventUpdate& event)
    {
        update(event.data.data);
    });
}

AnimationSystem::~AnimationSystem()
{
}

void AnimationSystem::update(double dt)
{
    for(auto& delegate : m_controller->iterate<AnimationComponent>())
    {
        sp::entity_type         entity    = delegate.first;
        AnimationComponent& animation = delegate.second;

        int state = animation.state;
        if(animation.frames.find(state) == animation.frames.end())
            continue;

        std::vector<sp::recti>& frames = animation.frames[state];
        auto& sprite_comp = m_controller->getComponent<SpriteComponent>(entity);

        /*
        if(!sprite_comp)
        {
            continue;
        }
        */

        std::weak_ptr<sp::Sprite> sprite = sprite_comp.sprite;
        if(sprite.expired() || !sprite.lock())
        {
            continue;
        }

        if(animation.playing)
        {
            animation.elapsed_time += dt;

            if(animation.elapsed_time > animation.frame_time)
            {
                animation.current_frame++;


                if(animation.current_frame >= frames.size())
                {
                    if(!animation.loop)
                    {
                        animation.playing = false;
                        sprite_comp.sprite->setVisible(false);
                        continue;
                    }
                    animation.current_frame = 0;
                }
                sp::recti& frame = frames[animation.current_frame];//animation.frames[animation.current_frame];

                //printf("frame: %d %d %d %d\n", frame.left, frame.top, frame.width, frame.height);
                sprite.lock()->setTextureRect(frame);
                animation.elapsed_time = 0.f;
            }
        }
    }
}

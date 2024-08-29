#ifndef ACTOR_H_INCLUDED
#define ACTOR_H_INCLUDED
#include <sp/ecs/machine.h>
#include <sp/arb/levler.h>
#include <pacman/directions.h>
#include <pacman/ecs/sprite_component.h>
#include <pacman/ecs/position_component.h>
#include <pacman/ecs/animation_component.h>

namespace sp
{
    class Controller;
}

//int tell_dir(const sp::vec2f& from, const sp::vec2f& to);
class Entity
{
    public:

        Entity();
       ~Entity();
        void    setSprite(sp::Sprite::Ptr sprite);
        void    setFrameDuration(float duration);
        void    setLoopAnimation(bool loop);
        void    addAnimationFrame(const sp::recti& area, int state);
        void    setAnimation(int state);
        void    setEntitySpeed(float speed);
        void    setController(std::shared_ptr<sp::Controller> ctrl);
        void    setPosition(const sp::vec2f& pos);
        void    setPosition(float x, float y);
        void    setTexture(const sp::Texture& texture);
        void    setMap(sp::Levler& map);
        void    moveEntity();
        void    add();

        sp::entity_type registerComponents();
        sp::entity_type getEntityID() const;
        virtual void setVulnerable() = 0;
        bool isVulnerable() const;
        virtual void reset() = 0;

        sp::vec2f getEntityPosition() const;
        virtual void    setStartPosition(const sp::vec2f& pos);
                void    setStartPosition(float x, float y);


        //void setTargetPosition(const sp::vec2f& pos);


    protected:
        virtual void    resolveIndirection() = 0;
        virtual void    resolveOOB() = 0;
        virtual void    moveImpl() = 0;

        bool isOOB(sp::vec2f& pos, sp::vec2f& next);
        sp::vec2f       getPosition(const sp::vec2f& pos, int direction);

        sp::vec2f       getCoords(int index);
        int             getIndex(const sp::vec2f& global);
        int             opposite(int direction);

        sp::vec2i       getGridCoords(const sp::vec2f& global);


        int             m_lead_direction;
        sp::vec2f       m_start_pos;


        std::weak_ptr<sp::Controller> m_controller;

        unsigned int        m_unit_length;
        const sp::Texture*  m_texture;
        sp::Levler&         m_map;
        sp::entity_type     m_entity_id;
        sp::vec2f           m_current_pos;
        bool                m_vulnerable;

        SpriteComponent     m_sprite_component;
        PositionComponent   m_position_component;
        AnimationComponent  m_animation_component;
};

#endif // ACTOR_H_INCLUDED

#include <pacman/actor.h>
#include <sp/sp_controller.h>

namespace
{
    static sp::Levler fallback;
}
Entity::Entity() :
    m_map           {fallback},
    m_unit_length   {16},
    m_entity_id     {0},
    m_start_pos     {0, 0},
    m_current_pos   {0, 0},
    m_texture       {nullptr},
    m_lead_direction{0},
    m_vulnerable    {false}
{
    m_animation_component.playing = true;
    m_animation_component.current_frame = 0;
    m_animation_component.elapsed_time  = 0.f;
    m_animation_component.loop = true;
}

Entity::~Entity()
{
    if(m_controller.expired() || !m_controller.lock())
        return;

    if(m_controller.lock()->valid(m_entity_id))
    {
        m_controller.lock()->remove<PositionComponent>(m_entity_id);
        m_controller.lock()->remove<AnimationComponent>(m_entity_id);
        m_controller.lock()->remove<SpriteComponent>(m_entity_id);
    }
}

void Entity::setTexture(const sp::Texture& texture)
{
    m_texture = &texture;
}

int Entity::opposite(int direction)
{
    switch(direction)
    {
        case DIR_LEFT:
            return DIR_RIGHT;
        case DIR_RIGHT:
            return DIR_LEFT;
        case DIR_UP:
            return DIR_DOWN;
        case DIR_DOWN:
            return DIR_UP;
    }
}

void Entity::setAnimation(int state)
{
    if(m_controller.lock()->valid(m_entity_id))
    {
        auto& anim = m_controller.lock()->getComponent<AnimationComponent>(m_entity_id);
        anim.state = state;
    }
    else
    {
        m_animation_component.state = state;
    }
}

sp::entity_type Entity::registerComponents()
{
    if(!m_controller.lock() || m_controller.expired())
    {
        return -1;
    }
    m_entity_id = m_controller.lock()->createEntity();

    m_sprite_component.sprite = sp::Sprite::create();
    if(m_texture)
        m_sprite_component.sprite->setTextureSprite(*m_texture);

    sp::vec2f origin = {m_unit_length / 2, m_unit_length / 2};
    sp::vec2f size   = {m_unit_length, m_unit_length};

    m_position_component.origin = origin;
    m_position_component.size = size;

    m_sprite_component.sprite->setSize(size);
    m_sprite_component.sprite->setOrigin(origin);
    m_sprite_component.sprite->setPosition(m_start_pos);

    m_controller.lock()->addDrawable(m_sprite_component.sprite, true);

    m_controller.lock()->assign<AnimationComponent>(m_entity_id, std::move(m_animation_component));
    m_controller.lock()->assign<SpriteComponent>(m_entity_id, std::move(m_sprite_component));
    m_controller.lock()->assign<PositionComponent>(m_entity_id, std::move(m_position_component));

    return m_entity_id;
}

void Entity::setController(std::shared_ptr<sp::Controller> ctrl)
{
    m_controller = ctrl;
}
void Entity::setMap(sp::Levler& map)
{
    m_map = map;
    m_unit_length = map.getDrawableUnitLength();
}
void Entity::setSprite(sp::Sprite::Ptr sprite)
{
    if(!sprite) return;
    m_sprite_component.sprite = sprite;
}

void Entity::setFrameDuration(float duration)
{
    m_animation_component.frame_time = duration;
}

void Entity::setLoopAnimation(bool loop)
{
    m_animation_component.loop = loop;
}

void Entity::addAnimationFrame(const sp::recti& area, int state)
{
    m_animation_component.state = state;
    m_animation_component.frames[state].push_back(area);
}

void Entity::setStartPosition(const sp::vec2f& pos)
{
    m_start_pos = m_position_component.last_position = m_position_component.next_position = pos;
}

void Entity::setStartPosition(float x, float y)
{
    setStartPosition(sp::vec2f{x, y});
}

void Entity::setEntitySpeed(float speed)
{
    m_position_component.speed = speed;
}

sp::vec2f Entity::getPosition(const sp::vec2f& pos, int direction)
{
    sp::vec2f available[4] =
    {
        {pos.x - m_unit_length, pos.y},
        {pos.x + m_unit_length, pos.y},
        {pos.x, pos.y - m_unit_length},
        {pos.x, pos.y + m_unit_length}
    };

    return available[direction];
}

bool Entity::isOOB(sp::vec2f& pos, sp::vec2f& next)
{
    sp::vec2i grid = getGridCoords(pos);
    sp::vec2u map_size = m_map.getMapSize();
    const unsigned int unit = m_map.getDrawableUnitLength();

    if(!grid.x && m_lead_direction == DIR_LEFT)
    {
        next = {pos.x - unit, pos.y};
        return true;
    }

    if(grid.x == (map_size.x - 1) && m_lead_direction == DIR_RIGHT)
    {
        next = {pos.x + unit, pos.y};
        return true;
    }

    if(!grid.y && m_lead_direction == DIR_UP)
    {
        next = {pos.x, pos.y - unit};
        return true;
    }

    if(grid.y == (map_size.y - 1) && m_lead_direction == DIR_DOWN)
    {
        next = {pos.x, pos.y + unit};
        return true;
    }


    if(m_map.isOutOfBounds(pos))
    {
        printf("out of bounds..\n");
        switch(m_lead_direction)
        {
            case DIR_LEFT:
            {
                pos  = {pos.x + unit * (map_size.x + 1), pos.y};
                next = {pos.x - unit, pos.y};
                break;
            }
            case DIR_RIGHT:
            {
                pos  = {pos.x - unit * (map_size.x + 1), pos.y};
                next = {pos.x + unit, pos.y};
                break;
            }
            case DIR_UP:
            {
                pos  = {pos.x , pos.y - unit * (map_size.y + 1)};
                next = {pos.x, pos.y - unit};
                break;
            }
            case DIR_DOWN:
            {
                pos  = {pos.x, pos.y + unit * (map_size.y + 1)};
                next = {pos.x, pos.y + unit};
                break;
            }
        }

        return true;
    }
    return false;
}

int Entity::getIndex(const sp::vec2f& global)
{
    static sp::vec2u map_size = m_map.getMapSize();
    sp::vec2i output     = m_map.mapToViewport(global);
    sp::vec2i indexed    = m_map.mapToGrid(output);

    return  indexed.y * map_size.x + indexed.x;
}

sp::vec2i Entity::getGridCoords(const sp::vec2f& global)
{
    sp::vec2i output = m_map.mapToViewport(global);
    return m_map.mapToGrid(output);
}

sp::vec2f Entity::getCoords(int index)
{
    const sp::vec2u map_size = m_map.getMapSize();
    const unsigned int unit = m_map.getDrawableUnitLength();
    int _x = index % map_size.x;
    int _y = index / map_size.x;

    sp::vec2f result = sp::vec2f{_x, _y} * unit + m_map.getOrigin();
    return {result.x + unit / 2, result.y + unit / 2};
}
void Entity::setPosition(const sp::vec2f& pos)
{
    if(!m_controller.lock() || m_controller.expired())
        return;
    /*PositionComponent*/
    auto& posComp = m_controller.lock()->getComponent<PositionComponent>(m_entity_id);
    posComp.last_position = posComp.next_position = pos;
}

void Entity::setPosition(float x, float y)
{
    setPosition(sp::vec2f{x, y});
}

sp::vec2f Entity::getEntityPosition() const
{
    return m_current_pos;
}

int _tell_dir(const sp::vec2f& from, const sp::vec2f& to)
{
    if(to.x < from.x)
        return DIR_LEFT;
    else if(to.x > from.x)
        return DIR_RIGHT;
    else if(to.y < from.y)
        return DIR_UP;
    else if(to.y > from.y)
        return DIR_DOWN;

    return -1;
}

bool Entity::isVulnerable() const
{
    return m_vulnerable;
}

void Entity::moveEntity()
{
    if(!m_controller.lock() || m_controller.expired())
        return;

    auto& posComp = m_controller.lock()->getComponent<PositionComponent>(m_entity_id);
    m_current_pos = posComp.last_position;

    if(posComp.last_position != posComp.next_position)
    {
        resolveIndirection();
        return;
    }

    if(isOOB(posComp.last_position, posComp.next_position))
    {
        resolveOOB();
        return;
    }

    moveImpl();
    auto& anim_comp   = m_controller.lock()->getComponent<AnimationComponent>(m_entity_id);
    if(!m_vulnerable)
    {
        int dir = _tell_dir(posComp.last_position, posComp.next_position);
        if(dir != anim_comp.state && dir != -1)
            anim_comp.state = dir;
    }
}

sp::entity_type Entity::getEntityID() const
{
    return m_entity_id;
}

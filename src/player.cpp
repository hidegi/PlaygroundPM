#include <pacman/player.h>
#include <sp/sp_controller.h>

#define KEY_A     65
#define KEY_S     83
#define KEY_W     87
#define KEY_D     68

#define SET_ALL_X(array, b)   \
    array[DIR_LEFT]  =   \
    array[DIR_UP]    =   \
    array[DIR_DOWN]  =   \
    array[DIR_RIGHT] = b

Player::Player() :
    Entity(),
    m_lead_key       {-1},
    m_hint_direction {0},
    m_overlay_count  {0}
{
    SET_ALL_X(m_directions, false);
    SET_ALL_X(m_vkeys, false);

}

#define VIRTUAL_SET(query, token)                        \
    if(key == (query))                                   \
    {                                                    \
        m_vkeys[(token)] = toggle;                       \
        if(m_lead_key != -1)                             \
        {                                                \
            if(!m_vkeys[m_lead_key])                     \
            {                                            \
                m_directions[m_lead_key] = false;        \
                m_lead_key = (token);                    \
            }                                            \
            else                                         \
            {                                            \
                                                         \
                m_directions[(token)] = toggle;          \
            }                                            \
        }                                                \
        else                                             \
        {                                                \
            m_directions[m_lead_key = (token)] = toggle; \
        }                                                \
    }

void Player::setKey(unsigned int key, bool toggle)
{
    VIRTUAL_SET(KEY_A, DIR_LEFT)
    VIRTUAL_SET(KEY_D, DIR_RIGHT)
    VIRTUAL_SET(KEY_W, DIR_UP)
    VIRTUAL_SET(KEY_S, DIR_DOWN)

    if(m_lead_key != -1)
    {
        m_directions[m_lead_key] = true;
    }
}

void Player::resolveIndirection()
{
    auto& posComp = m_controller.lock()->getComponent<PositionComponent>(m_entity_id);
    sp::vec2f target = posComp.next_position;
    sp::vec2f available[4] =
    {
        {target.x - m_unit_length, target.y},
        {target.x + m_unit_length, target.y},
        {target.x, target.y - m_unit_length},
        {target.x, target.y + m_unit_length}
    };
    for(size_t i = 0; i < 4; i++)
    {
        if(m_directions[i] && i != m_lead_direction)
        {
            SET_ALL_X(m_directions, false);
            m_hint_direction = i;

            m_directions[m_lead_direction] = true;
            m_directions[i] = true;
            break;
        }
    }
}

bool Player::process(const sp::vec2f& position, const sp::vec2f& target, int direction, sp::vec2f& result, bool axis)
{
    /*
    if(m_lead_direction == -1)
    {
        result = position;
        return true;
    }
    */
    unsigned int unit    = m_unit_length;

    sp::vec2f hinter = getPosition(position, m_hint_direction);
    const sp::Tile& main_tile = m_map.getTile(target);
    const sp::Tile& hint_tile = m_map.getTile(hinter);

    bool walkable        = main_tile.walkable && (main_tile.id != 21);//m_map.walkable(target);


    bool hint_possible = hint_tile.walkable && (hint_tile.id != 21);

    if(walkable)
    {
        if(m_lead_direction != direction)
        {
            SET_ALL_X(m_directions, false);
            m_directions[direction] = true;
            m_hint_direction        = direction;
        }

        bool pass = m_hint_direction == direction ? true : !hint_possible;
        //condition..
        if(pass)
        {
            m_directions[m_lead_direction = direction] = true;
            if(axis)
            {
                result.x = target.x;//m_map.getOrigin().x + (unitized.x + 1) * unit - unit / 2;
            }
            else
            {
                result.y = target.y;//m_map.getOrigin().y + (unitized.y + 1) * unit - unit / 2;
            }
			return true;
        }
        else
        {
            if(hint_possible)
            {

                m_directions[m_lead_direction] = false;
                m_directions[m_lead_direction = m_hint_direction] = true;
                if(axis)
                {
                    result.x = hinter.x;
                }
                else
                {
                    result.y = hinter.y;
                }

                return true;
            }
        }
    }
    else
    {
        if(m_lead_direction != direction && direction != opposite(m_lead_direction))
        {
            SET_ALL_X(m_directions, false);
            m_directions[m_lead_direction] =
            m_directions[m_hint_direction  = direction] = true;
        }
        else
        {
            if(hint_possible)
            {

                m_directions[m_lead_direction] = false;
                m_directions[m_lead_direction = m_hint_direction] = true;
                if(axis)
                {
                    result.x = hinter.x;
                }
                else
                {
                    result.y = hinter.y;
                }

                return true;
            }
        }
    }

	return false;
}

void Player::moveImpl()
{
    auto& posComp = m_controller.lock()->getComponent<PositionComponent>(m_entity_id);
    if(!m_map.isOutOfBounds(posComp.next_position))
    {
        const sp::Tile& tile = m_map.getTile(posComp.next_position);
        int index = m_map.getIndex(posComp.next_position);
        //sp::vec2u map_size = m_map.getMapSize();
        if(m_map.isVisible(index))
        {
            ++m_overlay_count;
            unsigned int total_count = m_map.getOverlayDrawableCount();

            if(tile.id == 1)
            {
                m_controller.lock()->getSubject()->dispatch(PointEvent{});
            }
            if(tile.id == 2)
            {
                m_controller.lock()->getSubject()->dispatch(PowerEvent{});
            }
            m_map.setVisible(index, false);

            if(m_overlay_count == total_count)
            {
                printf("YOU WIN!!");
                m_overlay_count = 0;
                m_controller.lock()->getSubject()->dispatch(WinEvent{});
            }
        }
    }

    unsigned int unit  = m_unit_length;
    sp::vec2f position = posComp.last_position;


    bool processed = false;
    if((m_directions[DIR_LEFT]) && !processed) //A
    {
        sp::vec2f targetPos = {position.x - unit, position.y};
        processed = process(position, targetPos, DIR_LEFT, posComp.next_position, true);
    }

    if((m_directions[DIR_RIGHT]) && !processed) //D
    {
        sp::vec2f targetPos = {position.x + unit, position.y};
        processed = process(position, targetPos, DIR_RIGHT, posComp.next_position, true);
    }

    if((m_directions[DIR_UP]) && !processed)  //W
    {
        sp::vec2f targetPos = {position.x, position.y - unit};
        processed = process(position, targetPos, DIR_UP, posComp.next_position, false);
    }

    if((m_directions[DIR_DOWN]) && !processed) //S
    {
        sp::vec2f targetPos = {position.x, position.y + unit};
        processed = process(position, targetPos, DIR_DOWN, posComp.next_position, false);
    }
}

void Player::resolveOOB()
{
}

void Player::setVulnerable()
{
    m_vulnerable = true;
}

void Player::reset()
{
    auto& posComp    = m_controller.lock()->getComponent<PositionComponent>(m_entity_id);
    auto& spriteComp = m_controller.lock()->getComponent<SpriteComponent>(m_entity_id);
    auto& animComp   = m_controller.lock()->getComponent<AnimationComponent>(m_entity_id);

    posComp.last_position = posComp.next_position = m_start_pos;
    spriteComp.sprite->setPosition(m_start_pos);

    SET_ALL_X(m_directions, false);
    m_lead_direction = m_hint_direction = m_lead_key = animComp.state = DIR_LEFT;
    animComp.loop    = true;
    animComp.playing = true;
    spriteComp.sprite->setVisible(true);

    m_vulnerable = false;

}


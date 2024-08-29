#include <pacman/ghost.h>
#include <sp/sp_controller.h>
#include <bits/stdc++.h>

namespace
{
    bool vecEqual(const sp::vec2f& v1, const sp::vec2f& v2)
    {
        return !spHelperCompareFloat(sp::vec2f{v1 - v2}.length(), 0.f);
    }
}
Ghost::Ghost() :
    m_target_changed        {true},
    m_target_pos            {0, 0},
    m_max_start_up_time     {5.f},
    m_start_up_movement     {true},
    m_killed                {false},
    m_reset_close_elapsed   {0.f},
    m_gate_pass             {false},
    m_cooldown_reached      {false},
    m_free_ghost            {false},
    m_initial_direction     {DIR_UP},
    m_easy_mode             {true}
{
    m_lead_direction = DIR_UP;
    m_stop_watch.reset();
}

void Ghost::reset()
{
    printf("ghost reset..\n");
    //only if killed = true..
    auto&  pos_component = m_controller.lock()->getComponent<PositionComponent>(m_entity_id);
    auto&  sprite_component = m_controller.lock()->getComponent<SpriteComponent>(m_entity_id);

    pos_component.last_position = pos_component.next_position = m_start_pos;
    sprite_component.sprite->setVisible(true);
    sprite_component.sprite->setPosition(m_start_pos);

    m_start_up_movement = !m_free_ghost;

    m_lead_direction = m_initial_direction;
    setAnimation(m_lead_direction);

    pos_component.speed = 80.f;
    m_stop_watch.reset();
    m_vulnerable = m_killed = false;
    m_gate_pass  = false;
}

void Ghost::setInitialDirection(int direction)
{
    m_initial_direction = direction;
    setAnimation(direction);
}
void Ghost::setTargetPoint(const sp::vec2f& pos)
{
    if(m_start_up_movement || m_killed)
      return;
    //auto& posComp = m_controller.lock()->getComponent<PositionComponent>(m_entity_id);
    m_target_pos = pos;
    m_target_changed = true;
}

bool Ghost::isKilled() const
{
    return m_killed;
}

void Ghost::setVulnerable()
{
    if((m_vulnerable && !m_cooldown_reached) || m_killed || m_start_up_movement)
        return;

    auto& anim_component = m_controller.lock()->getComponent<AnimationComponent>(m_entity_id);
    anim_component.state = VULNERABLE;
    m_stop_watch.reset();
    m_vulnerable = true;
    m_cooldown_reached = false;
}

void Ghost::setKilled()
{
    if(m_killed)
        return;
    m_gate_pass = false;
    //printf("gate pass reset..\n");

    auto&  pos_component    = m_controller.lock()->getComponent<PositionComponent>(m_entity_id);
    pos_component.speed = 300;
    setTargetPoint(m_start_pos);
    m_killed = true;
    //m_reset_close_elapsed = m_stop_watch.getElapsedSeconds();
}

int tell_dir(const sp::vec2f& from, const sp::vec2f& to)
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

void Ghost::setStartUpDuration(float time)
{
    if(time >= 0.f)
        m_max_start_up_time = time;
}

void Ghost::moveImpl()
{
    auto& posComp = m_controller.lock()->getComponent<PositionComponent>(m_entity_id);
    sp::vec2f position = posComp.last_position;

    sp::vec2f available[4] =
    {
        {position.x - m_unit_length, position.y},
        {position.x + m_unit_length, position.y},
        {position.x, position.y - m_unit_length},
        {position.x, position.y + m_unit_length}
    };

    if(m_start_up_movement)
    {
        if(m_stop_watch.getElapsedSeconds() >= m_max_start_up_time)
        {
            m_start_up_movement = false;
            m_stop_watch.reset();
        }
        if(m_start_up_movement)
        {
            const sp::Tile& tile = m_map.getTile(available[m_lead_direction]);
            if(tile.walkable && tile.id != 21)
            {
                posComp.next_position = available[m_lead_direction];
            }
            else
            {

                m_lead_direction = opposite(m_lead_direction);
                posComp.next_position = available[m_lead_direction];
            }
        }
        return;
    }

    if(m_vulnerable && !m_killed)
    {
        auto& anim_component = m_controller.lock()->getComponent<AnimationComponent>(m_entity_id);
        float current_elapsed = m_stop_watch.getElapsedSeconds();
        static float last_elapsed = current_elapsed;

        if(current_elapsed >= MAX_VULNERABILITY_TIME - 3)
        {
            m_cooldown_reached = true;
            if((current_elapsed - last_elapsed) >= .1f)
            {

                int state = anim_component.state;
                if(state == COOLDOWN)
                    anim_component.state = VULNERABLE;
                else
                    anim_component.state = COOLDOWN;

                last_elapsed = current_elapsed;
            }
        }

        if(current_elapsed >= MAX_VULNERABILITY_TIME)
        {
            last_elapsed = MAX_VULNERABILITY_TIME - 3.f;
            m_vulnerable = false;
        }
    }



    const sp::Tile& tile = m_map.getTile(posComp.last_position);
    if(tile.id == 21 && !m_vulnerable)
    {
        m_gate_pass = true;
        printf("gate pass..\n");
    }

    bool optional = false;
    for(size_t i = 0; i < 4 && !optional; i++)
    {
        if(i == opposite(m_lead_direction) || i == m_lead_direction)
        {
            continue;
        }

        if(m_map.walkable(available[i]))
        {
            optional = true;
            break;
        }
    }

    if(((m_vulnerable && !m_killed) || m_easy_mode) && m_gate_pass)
    {
        if(optional)
            posComp.next_position = simple(posComp.last_position, m_target_pos);
        else
            posComp.next_position = available[m_lead_direction];
    }
    else
    {
        if(optional || m_killed)
        {

            if(m_killed && sp::vec2f(posComp.last_position - m_start_pos).length() <= 0.f)
            {
                printf("reset here start..\n");
                reset();
            }
            posComp.next_position = hard(posComp.last_position, m_target_pos);
        }
        else
        {
            posComp.next_position = available[m_lead_direction];
        }
    }
}

void Ghost::resolveIndirection()
{
}

void Ghost::resolveOOB()
{
    m_closed_list.clear();
}

void Ghost::markAsFreeGhost(bool free)
{
    m_free_ghost = free;
    m_start_up_movement = !free;
}

float Ghost::calcHCost(const sp::vec2i& from, const sp::vec2i& to)
{
    const unsigned int unit = m_unit_length;
    const float diagonal_distance = std::sqrt(unit * unit + unit * unit);

    float dx = std::fabs(from.x - to.x);
    float dy = std::fabs(from.y - to.y);
    return (unit * (dx + dy) + (diagonal_distance - 2 * unit) * std::fminf(dx, dy));
}

void Ghost::setEasyMode(bool easy)
{
    m_easy_mode = easy;
}
sp::vec2f Ghost::hard(const sp::vec2f& current, const sp::vec2f& point)
{
    if(getIndex(current) == getIndex(point))
    {
        return current;
    }

    if(!m_map.walkable(point))
    {
        return current;
    }
    static auto comparator = [this](const Node& n1, const Node& n2)
    {
        return n1.cost_g > n2.cost_g;
    };
    std::priority_queue<Node, std::vector<Node>, decltype(comparator)>  open_list(comparator);
    //static std::m_map<int, Node>  m_closed_list;
    if(m_target_changed)
    {
        m_closed_list.clear();
        m_target_changed = false;
    }
    if(m_map.isOutOfBounds(current))
      m_closed_list.clear();

    const unsigned int unit = m_map.getDrawableUnitLength();
    float gcost = 1.f;
    float hcost = calcHCost(current, point);
    float fcost = gcost + hcost;

    int start_index = getIndex(current);
    int end_index   = getIndex(point);

    open_list.push({start_index, start_index, gcost, fcost});

    static bool found = false;
    if(m_closed_list.empty())
    {
        found = false;
        while(!open_list.empty())
        {
            Node q = open_list.top();
            open_list.pop();

            int parent = q.index;
            if(parent == end_index)
            {
                found = true;
                m_closed_list.insert({parent, q});
                break;
            }

            sp::vec2f best = getCoords(parent);
            sp::vec2f available[4] =
            {
                {best.x - unit, best.y},
                {best.x + unit, best.y},
                {best.x, best.y - unit},
                {best.x, best.y + unit}
            };

            for(size_t i = 0; i < 4; i++)
            {
                sp::vec2f n = available[i];
                int successor_index = getIndex(n);

                const sp::Tile& tile = m_map.getTile(n);
                if(!tile.walkable || (tile.id == 21 && m_gate_pass) || m_map.isOutOfBounds(n))
                    continue;

                float gcost = q.cost_g + 1.f;
                float hcost = calcHCost(n, point);
                float fcost = gcost + hcost;

                Node successor =
                {
                    parent,
                    successor_index,
                    gcost,
                    fcost
                };

                auto it = m_closed_list.find(successor_index);

                if(fcost < q.cost_f || it == m_closed_list.end())
                {
                    open_list.push(successor);
                }
                else
                {
                    if(it != m_closed_list.end())
                    {
                        Node& finder = it->second;
                        if(fcost < finder.cost_f)
                        {
                            open_list.push(successor);
                            m_closed_list[successor_index] = successor;
                        }
                    }
                    else
                    {
                        m_closed_list.insert({successor_index, successor});
                    }
                }
            }
            m_closed_list.insert({parent, q});
        }
    }

    if(!found)
        return current;
    int finder = end_index;
    while(finder != start_index)
    {
        auto next_it = m_closed_list.find(finder);
        if(next_it == m_closed_list.end())
        {
            printf("path not possible..\n");
            return current;
        }

        Node& current_node = next_it->second;
        if(current_node.parent == start_index)
        {
            sp::vec2f result = getCoords(current_node.index);
            m_lead_direction = tell_dir(current, result);
            return result;
        }
        finder = current_node.parent;
    }

    return current;
}

sp::vec2f Ghost::simple(const sp::vec2f& current, const sp::vec2f& point)
{
    const sp::vec2f& current_pos = current;
    //
    //printf("point: %f %f\n", point.x, point.y);
    unsigned int unit     =     m_unit_length;
    sp::vec2i output           = m_map.mapToViewport(point);
    sp::vec2i unitized_target  = m_map.mapToGrid(output) * unit + static_cast<sp::vec2i>(m_map.getOrigin());

    output = m_map.mapToViewport(current);
    sp::vec2i unitized_source  = m_map.mapToGrid(output) * unit + static_cast<sp::vec2i>(m_map.getOrigin());




    if(unitized_source == unitized_target)
    {
      //  printf("point: %f %f, last pos: %f %f\n", point.x, point.y, current_pos.x, current_pos.y);
        return {unitized_target.x + (unit / 2), unitized_target.y + (unit / 2)};
    }

    float total_distance  = sp::vec2f{point - current_pos}.length();


    sp::vec2f available[4] =
    {
        {current_pos.x - unit, current_pos.y},
        {current_pos.x + unit, current_pos.y},
        {current_pos.x, current_pos.y - unit},
        {current_pos.x, current_pos.y + unit}
    };

    /*
    sp::vec2i output = m_map.mapToViewport(current_pos);
    sp::vec2i unitized  = m_map.mapToGrid(output);
    */
    //printf("distance: %f\n", total_distance);
    sp::vec2f best  = current_pos;
    float shortest  = std::numeric_limits<float>::max();

    int previous            = m_lead_direction;

    float least_angle_x       = 45.f;
    float least_angle_y       = 45.f;

    float axis_x    = (current.x < point.x) ? -1.f : (current.x > point.x) ? 1.f : 0.f;
    float axis_y    = (current.y < point.y) ? -1.f : (current.y > point.y) ? 1.f : 0.f;

    sp::vec2f normal_axis_x = {axis_x, 0.f};
    sp::vec2f normal_axis_y = {0.f, axis_y};

    sp::vec2f normal_axes  = {axis_x, axis_y};
    int highscore = 0;
    int winner = previous;
    for(size_t i = 0; i < 4; i++)
    {
        sp::vec2i output = m_map.mapToViewport(available[i]);
        const sp::Tile& tile = m_map.getTile(available[i]);
        if(!tile.walkable || (tile.id == 21 && m_gate_pass) || m_map.isOutOfBounds(available[i]))
            continue;

        int current_direction = i;
        if(current_direction == opposite(previous))
            continue;

        int score = 0;
        float local_distance = sp::vec2f{point - available[i]}.length();
        if(local_distance <= shortest)
        {
               /*
               m_lead_direction == opposite(i)
               */
            score += 2;
            //m_target_changed = false;
            shortest = local_distance;
            //best = available[i];

            //m_lead_direction = i;
        }


        float inclination_x = std::fabs(point.x - available[i].x);
        float inclination_y = std::fabs(point.y - available[i].y);

        sp::vec2f normalized = available[i].normalized();
        if(inclination_x < inclination_y)
        {
            //x
            float angle = dot(normal_axis_x, normalized);
            if(angle < least_angle_x)
            {
                score += 1;
                least_angle_x = angle;
                //best = available[current_direction];
            }

            if(!spHelperCompareFloat(angle, 0.f))
                score += 2;
        }
        else if(inclination_y < inclination_x)
        {
            float angle = dot(normal_axis_y, normalized);
            if(angle < least_angle_y)
            {
                score += 1;
                least_angle_y = angle;
                //best = available[current_direction];
            }

            if(!spHelperCompareFloat(angle, 0.f))
                score += 2;

        }
        else
        {
            //target found..
            score += 5;
        }

        if(score >= highscore)
        {
            highscore = score;
            winner = current_direction;
            //m_lead_direction = current_direction;
        }
    }

    best = available[winner];
    m_lead_direction = winner;

    if(!highscore)
    {
        best = available[opposite(previous)];
        m_lead_direction = opposite(previous);
        return best;
    }

    return best;
}

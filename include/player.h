#ifndef PACMAN_PLAYER_H
#define PACMAN_PLAYER_H
#include <pacman/actor.h>

class Player : public Entity
{
    public:
        Player();

        void        setKey(unsigned int key, bool toggle);
        void        setVulnerable() override;
        void        reset() override;
    private:
        virtual void resolveIndirection() override;
        virtual void resolveOOB() override;
        virtual void moveImpl() override;

        bool process(const sp::vec2f& position, const sp::vec2f& target, int direction, sp::vec2f& result, bool axis);

        unsigned int    m_overlay_count;
        int             m_hint_direction;
        int             m_lead_key;
        bool            m_directions[4];
        bool            m_vkeys[4];
};

#endif // PLAYER_H

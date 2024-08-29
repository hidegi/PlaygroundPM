#ifndef PACMAN_GHOST_H
#define PACMAN_GHOST_H
#include <pacman/actor.h>
#include <sp/utils/stopwatch.h>

class Ghost : public Entity
{
    public:
        Ghost();

        void        setTargetPoint(const sp::vec2f& point);

        void        setVulnerable() override;
        void        reset() override;

        void        setKilled();
        bool        isKilled() const;

        void        setStartUpDuration(float time);
        void        markAsFreeGhost(bool free);
        void        setInitialDirection(int direction);
        void        setEasyMode(bool easy);
    private:
        virtual void moveImpl() override;
        virtual void resolveOOB() override;
        virtual void resolveIndirection() override;

        struct Node
        {
            int         parent;
            int         index;
            float       cost_f;
            float       cost_g;
        };

        float       calcHCost(const sp::vec2i& from, const sp::vec2i& to);

        sp::vec2f   simple(const sp::vec2f& current, const sp::vec2f& point);
        sp::vec2f   hard(const sp::vec2f& current, const sp::vec2f& point);

        float                   m_max_start_up_time;
        float                   m_elapsed_start_up_time;

        bool                    m_start_up_movement;
        int                     m_initial_direction;

        std::map<int, Node>     m_closed_list;

        bool                    m_target_changed;
        bool                    m_killed;
        bool                    m_gate_pass;
        bool                    m_cooldown_reached;
        bool                    m_free_ghost;
        bool                    m_easy_mode;

        sp::vec2f               m_target_pos;

        sp::StopWatch           m_stop_watch;
        float                   m_reset_close_elapsed;
        static inline float MAX_VULNERABILITY_TIME = 10.f;
};
#endif // GHOST_H

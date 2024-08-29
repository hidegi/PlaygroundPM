#ifndef SP_PAC_MAN
#define SP_PAC_MAN
#include <sp/sp_controller.h>
#include <sp/arb/levler.h>
#include <sp/arb/levels.h>
#include <sp/arb/overlay.h>
#include <sp/gxsp/sprite.h>
#include <sp/gxsp/label.h>
#include <sp/sp_listener.h>
#include <pacman/levelmap.h>
#include <pacman/ecs/animation_component.h>
#include <pacman/ecs/sprite_component.h>
#include <pacman/ecs/position_component.h>
#include <pacman/pacman_level.h>
#include <pacman/actor.h>
#include <pacman/player.h>
#include <pacman/ghost.h>

class PacMan final : public sp::Listener
{
    public:
                    PacMan() = delete;
                    PacMan(unsigned int width, unsigned int height);
                   ~PacMan();

        void        init();
        void        mainLoop();
        void        terminate();
    private:
        void            processInput(double dt);

        /*
        void            keyEvent(sp::SP_Detail* detail);
        void            update(sp::SP_Detail* detail);
        */

        bool            noPendingDirections(const sp::vec2f& query, bool x_axis, int hint_direction);


        void            processKey(const sp::vec2f& current, const sp::vec2f& target, int direction, sp::vec2f& output, bool axis);
        void            update(double dt);
        void            cursorPos(double x, double y);

        void            keyEvent(int key, int scancode, int action, int mods);
        void            haltGame();

        void            handleCollision(sp::entity_type type);
        void            handleWin();
        void            handlePoint();

        void            resetGhost();
        sp::vec2f       playerMoveDebug(const sp::vec2f& current, const sp::vec2f& point);

    private:
        sp::Levler map;
        std::shared_ptr<sp::Controller> m_controller;
        LevelMap        m_level_map;
        sp::Texture     texture;
        sp::Texture     motex;
        sp::Texture     m_level_texture;
        sp::Sprite::Ptr m_sprite;

        sp::Observer::Ptr           m_observer;

        sp::SpriteBatch::Ptr    m_sprite_batch;
        unsigned int    m_width;
        unsigned int    m_height;

        int             m_last_direction{-1};
        int             m_hint_direction{-1};

        bool            m_keys[1024];
        bool            m_directions[4];
        bool            m_terminated{false};

        sp::Sprite::Ptr             m_player_sprite;
        //Entity m_player;

        sp::vec2f                   m_target_point;
        Board                       m_board;

        Player                      m_player;
        //Ghost                       m_ghost_1;

        std::vector<Ghost*>         m_ghosts;
        sp::Font::Ptr               m_arcade_font;
        sp::StopWatch               m_halt_timer;
        sp::Label::Ptr              m_tooltip_label;
        sp::Label::Ptr              m_score_label;
        sp::entity_type             m_killed_ghost;

        sp::Sprite::Ptr             m_debug_sprite;
        sp::Sprite::Ptr             m_debug_sprite_2;

        sp::Texture                 m_motex_texture;
        sp::Texture                 m_motex_texture_2;

        bool                        m_screen_halt;
        bool                        m_run_game;
        bool                        m_player_win;

        int                         m_score;
        /*
        sp::entity_type             m_player;

        AnimationComponent          m_animation_component;
        SpriteComponent             m_sprite_component;
        PositionComponent           m_position_component;
        */
};

#endif

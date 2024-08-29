#ifndef PACMANLEVEL_H
#define PACMANLEVEL_H
#include <sp/sp_controller.h>
#include <sp/arb/levler.h>

namespace sp
{
    class Controller;
}
class Board
{
    public:
        Board();
       ~Board();

        void build(const void* data, const sp::Color& color = {255, 255, 255, 255});
        void setController(std::shared_ptr<sp::Controller> ctrl);
        void setRulePath(const char* path);
        void setSourceTexture(const sp::Texture& texture);

        //void setEnemySpawn(const sp::vec2f& pos, entity_type_t );
        //void setPlayerSpawn(const sp::vec2f& pos);
        void addLevel();
        void removeLevel();
        void nextLevel();

        sp::Levler& getCurrentMap();

    private:

        std::weak_ptr<sp::Controller> m_controller;
        std::vector<sp::Levler*> m_levels;
        std::vector<sp::Sprite::Ptr>    m_sprites;

        sp::Texture             m_motexture;
        const sp::Texture*      m_texture_atlas;
        sp::String              m_ruleset_path;
        int                     m_level_index;
};

#endif // PACMANLEVEL_H

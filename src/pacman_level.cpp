#include <pacman/pacman_level.h>
#include <sp/sp_controller.h>

Board::Board() :
    m_level_index   {0},
    m_texture_atlas {nullptr},
    m_ruleset_path  {}
{
}

Board::~Board()
{
    for(sp::Levler* level : m_levels)
        delete level;
}

void Board::setController(std::shared_ptr<sp::Controller> ctrl)
{
    if(!ctrl) return;
    m_controller = ctrl;
}

sp::Levler& Board::getCurrentMap()
{
    return *m_levels[m_level_index];
}

void Board::setRulePath(const char* path)
{
    m_ruleset_path = sp::String(path);
}

void Board::setSourceTexture(const sp::Texture& texture)
{
    m_texture_atlas = &texture;
}

void Board::build(const void* data, const sp::Color& color)
{
    if(!m_controller.lock() || m_controller.expired() || m_ruleset_path.empty() || !m_texture_atlas)
    {
        fprintf(stderr, "cannot build level while parameters not set..\n");
        return;
    }
    m_levels.push_back(new sp::Levler);
    sp::Levler& level = *m_levels.back();
    level.loadRuleSet(m_ruleset_path.toStdString().c_str());
    level.setSourceTexture(*m_texture_atlas);
    level.loadLevel(data);
    level.getOutputSprite()->setColor(color);
}

void Board::addLevel()
{
    if(++m_level_index >= m_levels.size())
        m_level_index = 0;
    //m_levels[m_level_index].getOutputSprite()->setZOrder(0);
    m_controller.lock()->addDrawable(m_levels[m_level_index]->getOutputSprite(), true);

}

void Board::removeLevel()
{
    m_controller.lock()->removeDrawable(m_levels[m_level_index]->getOutputSprite());
}

void Board::nextLevel()
{
    if(!m_controller.lock() || m_controller.expired())
        return;

    unsigned int previous = m_level_index;
    if(++m_level_index >= m_levels.size())
        m_level_index = 0;

    printf("previous %d, current: %d\n", previous, m_level_index);


    m_controller.lock()->removeDrawable(m_levels[previous]->getSpriteBatch());
    m_controller.lock()->removeDrawable(m_levels[previous]->getOutputSprite());

    //m_levels[m_level_index].getSpriteBatch()->setZOrder(0);
    m_levels[m_level_index]->getOutputSprite()->setZOrder(0);
    m_levels[m_level_index]->getSpriteBatch()->uniformVisibility(true);
    m_levels[m_level_index]->getSpriteBatch()->setZOrder(0);
    m_controller.lock()->addDrawable(m_levels[m_level_index]->getSpriteBatch());
    m_controller.lock()->addDrawable(m_levels[m_level_index]->getOutputSprite());

}

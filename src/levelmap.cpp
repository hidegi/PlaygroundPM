#include <pacman/levelmap.h>

LevelMap::LevelMap() :
    m_bounds    {0.f, 0.f, 1.f, 1.f},
    m_matrix    {},
    m_inv_mat   {}
{
}
/*
 *  NDC -> Pixels:
 *
 *  float pixelX = (NDCx + 1.0f) * 0.5 * screenWidth;
 *  float pixelY = (1.0f - NDCy) * 0.5 * screenHeight;
 */

bool LevelMap::walkable(float x, float y) const
{
    if(     x < 0.f || x > m_bounds.width
       ||   y < 0.f || y > m_bounds.height)
        return false;

    int width   = (int)(std::floor(m_bounds.width  / 16.f + .5f));
    int height  = (int)(std::floor(m_bounds.height / 16.f + .5f));

    int _x = sp::clamp((int)(std::floor((x - 8.f) / 16.f + .5f )), 0, width) ;
    int _y = sp::clamp((int)(std::floor((y - 8.f) / 16.f + .5f )), 0, height);
    int index = _y * width + _x;
    if(index >= width * height)
        return false;

    printf("index: %d, x: %d y: %d\n", index, _x, _y);
    return true;
}

bool LevelMap::walkable(const sp::vec2f& pos) const
{
    return walkable(pos.x, pos.y);
}

sp::vec2f LevelMap::mapPixels(const sp::vec2f& pos)
{
    sp::vec2f normalized;
    normalized.x = -1.f + 2.f * (pos.x - m_bounds.left) / m_bounds.width;
    normalized.y =  1.f - 2.f * (pos.y - m_bounds.top)  / m_bounds.height;

    setPosition(m_bounds.left, m_bounds.top);
    return Transformable::getInverseMatrix().transformVertex(normalized);
}

sp::vec2f LevelMap::mapToViewport(const sp::vec2f& pos)
{
    sp::vec2f ndc = mapPixels(pos);
    sp::vec2f out = {(ndc.x + 1.f) * .5f * m_bounds.width, (1.f - ndc.y) * .5f * m_bounds.height};
    return out;
}

sp::vec2i LevelMap::mapCoords(const sp::vec2f& pos)
{
    setPosition(0.f, 0.f);
    sp::vec2f normalized = Transformable::getMatrix().transformVertex(pos);

    sp::vec2i pixel;

    pixel.x = static_cast<int>(std::floor(( normalized.x + 1.f) / 2.f * m_bounds.width + m_bounds.left + .5f));
    pixel.y = static_cast<int>(std::floor((-normalized.y + 1.f) / 2.f * m_bounds.height + m_bounds.top + .5f));
    return pixel;
}

void LevelMap::setViewport(const sp::rectf& vp)
{
    m_bounds  = vp;
    setPosition(vp.left, vp.top);
}

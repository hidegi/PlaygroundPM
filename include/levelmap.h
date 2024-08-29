#ifndef LEVELMAP_H
#define LEVELMAP_H
#include <sp/gxsp/transformable.h>
#include <sp/math/vec.h>
#include <sp/math/rect.h>
#include <sp/math/mat.h>

class LevelMap : public sp::Transformable
{
    public:
        LevelMap();

        sp::vec2f mapToViewport(const sp::vec2f& pos);

        sp::vec2f mapPixels(const sp::vec2f& pos);
        sp::vec2i mapCoords(const sp::vec2f& pos);

        void setViewport(const sp::rectf& vp);
        bool walkable(const sp::vec2f& pos) const;
        bool walkable(float x, float y) const;

    private:

        sp::mat     m_matrix;
        sp::mat     m_inv_mat;
        sp::rectf   m_bounds;
};

#endif // LEVELMAP_H

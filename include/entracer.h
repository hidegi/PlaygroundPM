#ifndef PACMAN_ENTRACER_H
#define PACMAN_ENTRACER_H

#define DIR_LEFT  0
#define DIR_RIGHT 1
#define DIR_UP    2
#define DIR_DOWN  3


struct Node
{
    sp::vec2i       pos;    //grid coordinates..
    int             parent;
    int             index;
    float           hcost;
    float           gcost;
    int             ignore = -1;

    bool            walkable{true};
    bool            visited {false};
    sp::Levler&     map;

    Node(sp::Levler& map) :
        pos     {0, 0},
        parent  {-1}
        index   {-1},
        hcost   {0.f},
        gcost   {0.f}
        ignore  {-1},
        walkable{true},
        visited {false}
    {
    }

    explicit Node(sp::vec2i grid_pos) :
        Node    (),
        pos     {grid_pos},
    {
    }

    float fcost()
    {
        return (hcost + gcost);
    }

    void setParent(int parent)
    {
        this->parent = parent
    }
    std::vector<Node> getNeighbors()
    {
        std::vector<sp::vec2f> neighbors;
        if(ignore != DIR_LEFT)
        {
            neighbors.push_back()
        }

        if(ignore != DIR_RIGHT)
        {

        }

        if(ignore != DIR_UP)
        {

        }

        if(ignore != DIR_DOWN)
        {

        }
    }

    Node createNode(sp::vec2i pos)
    {

    }

};
#endif // ENTRACER_H

#include "PM/board.h"
#include <SP/gfx/framebuffer.h>
#include <SP/gfx/viewport.h>
#include <SP/gfx/ogl/gldraw.h>

namespace pm
{
	Board::Board() :
		m_current_level{0}
	{
	}
	Board::~Board()
	{
		for(auto* level : m_levels)
		{
			if(level->map)
				delete level->map;
			
			delete level;
		}
	}
	
	const sp::Texture* Board::getLevelTexture() const
	{
		if(m_levels.empty())
			return NULL;
		return &m_levels[m_current_level]->buffer;
	}
	
	sp::vec2u Board::getSize() const
	{
		return m_tex_atlas.size;
	}
	bool Board::loadFromFile(const char* path)
	{
		bool status = false;
		if((status = m_tree.loadFromFile(path)))
			status = status && init();
		
		return status;  
	}
	
	bool Board::loadFromFile(const std::string& path)
	{
		bool status = false;
		if(status = m_tree.loadFromFile(path.c_str()))
			status = status && init();
		return status;
	}
	
	bool Board::loadFromMemory(const void* memory, SPsize length)
	{
		bool status = false;
		if(status = m_tree.loadFromMemory(memory, length))
			status = status && init();
		return status;
	}
	
#define MEMBER_INIT(type, name, member)\
	type* name = dynamic_cast<type*>(m_tree.get(#name))
	

#define MEMBER_INIT_REQUIRED(type, name, member) do{\
	MEMBER_INIT(type, name, member);\
	if(!name) \
	{\
		SP_WARNING("Attribute \"%s\" not available", #name );\
		return false;\
	}\
	(member) = name->data;\
	}while(0)
		
#define MEMBER_INIT_OPTIONAL(type, name, member) do{\
	MEMBER_INIT(type, name, member);\
	if((name))\
	{\
		(member) = name->data;\
	}\
	}while(0)
		
		
	bool Board::init()
	{
		MEMBER_INIT_REQUIRED(sp::StringNode, texture, m_tex_atlas.path);	
		
		std::string fullpath = std::string(SP_WORKSPACE) + m_tex_atlas.path.c_str();
		if(!m_tex_atlas.texture.loadFromFile(fullpath.c_str()))
			return false;
		
		MEMBER_INIT_REQUIRED(sp::IntegerNode, width, m_tex_atlas.size.x);
		MEMBER_INIT_REQUIRED(sp::IntegerNode, height, m_tex_atlas.size.y);
		MEMBER_INIT_REQUIRED(sp::IntegerNode, unit, m_tex_atlas.unitLength);
		MEMBER_INIT_REQUIRED(sp::IntegerNode, padding, m_tex_atlas.padding);
		SP_ASSERT(m_tex_atlas.size.x > 0 && m_tex_atlas.size.y > 0, "Invalid texture size");
		
		sp::ListNode* tiles = dynamic_cast<sp::ListNode*>(m_tree.get("tiles"));
		SP_ASSERT(tiles, "Attribute \"tiles\" not available");
		
		for(sp::Node* n : tiles->data)
		{
			sp::ListNode* t = dynamic_cast<sp::ListNode*>(n);
			SP_ASSERT(t, "Attribute \"%s\" expected to be not empty", tiles->name);
			
			TileInfo tile;
			tile.id = -1;
			tile.walkable = tile.overlay = false;
			tile.texCoords.x = tile.texCoords.y = -1;
			for(sp::Node* tn : t->data)
			{
				if(strcmp(tn->name, "id") == 0)
				{
					tile.id = dynamic_cast<sp::IntegerNode*>(tn)->data;
				}
				
				if(strcmp(tn->name, "walkable") == 0)
				{
					tile.walkable = static_cast<bool>(dynamic_cast<sp::IntegerNode*>(tn)->data);
				}
				
				if(strcmp(tn->name, "overlay") == 0)
				{
					tile.overlay = static_cast<bool>(dynamic_cast<sp::IntegerNode*>(tn)->data);
				}
				
				if(strcmp(tn->name, "x") == 0)
				{
					tile.texCoords.x = static_cast<int>(dynamic_cast<sp::IntegerNode*>(tn)->data);
				}
				
				if(strcmp(tn->name, "y") == 0)
				{
					tile.texCoords.y = static_cast<int>(dynamic_cast<sp::IntegerNode*>(tn)->data);
				}
			}
			
			if(tile.id == -1)
			{
				SP_WARNING("Tile ID has not been specified");
				return false;
			}
			
			m_table.emplace(tile.id, tile);
		}
		
		sp::ListNode* levels = dynamic_cast<sp::ListNode*>(m_tree.get("levels"));
		SP_ASSERT(tiles, "Attribute \"levels\" not available");
		m_levels.reserve(levels->data.size());
		
		for(sp::Node* n : levels->data)
		{
			sp::IntegerArrayNode* grid = dynamic_cast<sp::IntegerArrayNode*>(n);
			SP_ASSERT(grid, "Attribute \"levels\" expected to be not empty");
			
			Level* level = new Level;
			
			level->map = new SPuint8[grid->data.size()];
			for(SPsize i = 0; i < grid->data.size(); i++)
				level->map[i] = grid->data[i] & 0xFF;
			render(level);
			m_levels.push_back(level);
		}
		
		return true;
	}
	
	void Board::render(Level* level)
	{
		SP_ASSERT(level, "Cannot render level with NULL");
		SP_ASSERT(level->map, "Level has uninitialized grid");
		SPuint8* grid = level->map;
		const float LEVEL_UNIT_LENGTH = 16;
		
		sp::gldraw::setGLStates();
		sp::Framebuffer* framebuffer = new sp::Framebuffer();
		SP_ASSERT(framebuffer->create(m_tex_atlas.size.x * LEVEL_UNIT_LENGTH, m_tex_atlas.size.y * LEVEL_UNIT_LENGTH), "Failed to create framebuffer");
		
		sp::Viewport viewport;
		viewport.setViewport(sp::rectf{0, 0, (float)m_tex_atlas.size.x * LEVEL_UNIT_LENGTH, (float)m_tex_atlas.size.y * LEVEL_UNIT_LENGTH});
		viewport.flipHorizontal(true);
		
		framebuffer->bind();
		sp::gldraw::clear();
		sp::Texture::bind(&m_tex_atlas.texture);
		viewport.load();
		for(SPsize i = 0; i < m_tex_atlas.size.x * m_tex_atlas.size.y; i++)
		{
			float pos_x = (i % m_tex_atlas.size.x) * LEVEL_UNIT_LENGTH;
			float pos_y = (i / m_tex_atlas.size.x) * LEVEL_UNIT_LENGTH;
			pm_id_t id = static_cast<pm_id_t>(level->map[i]);
			auto it = m_table.find(id);
			SP_ASSERT(it != m_table.end(), "Tile with id %lld has not been specified", id);
			TileInfo& tile = it->second;
			
			if(tile.overlay)
				continue;
			
			const unsigned int UNIT_LENGTH = m_tex_atlas.unitLength + m_tex_atlas.padding;
			const sp::vec2f SIZE = static_cast<sp::vec2f>(m_tex_atlas.texture.getSize());
			sp::gldraw::drawFrame(
				pos_x, 
				pos_y, 
				LEVEL_UNIT_LENGTH, 
				LEVEL_UNIT_LENGTH,
				tile.texCoords.x * UNIT_LENGTH / SIZE.x,
				tile.texCoords.y * UNIT_LENGTH / SIZE.y,
				UNIT_LENGTH / SIZE.x,
				UNIT_LENGTH / SIZE.y
			);
		}
		
		framebuffer->flush();
		level->buffer = framebuffer->copyToTexture();
		
		delete framebuffer;
	}
}
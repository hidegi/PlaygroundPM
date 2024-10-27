#ifndef PM_BOARD_H
#define PM_BOARD_H
#include "config.h"
#include <SP/gfx/texture.h>
#include <SP/sparse/extractor.h>
#include <SP/math/vec.h>
#include <map>

namespace pm
{
	class Board
	{
		public:
			Board();
		   ~Board();
		   
			bool loadFromFile(const char* path);
			bool loadFromFile(const std::string& path);
			bool loadFromMemory(const void* data, SPsize length);
			
			struct TileInfo
			{
				long long id;
				bool walkable;
				bool overlay;
				sp::vec2i texCoords;
			};
			
			sp::vec2u getSize() const;
			const sp::Texture* getLevelTexture() const;
			
		private:
			// initializer..
			bool init();
			
			struct TexAtlasInfo
			{
				std::string path;
				unsigned int padding;
				unsigned int unitLength;
				sp::vec2u size;
				sp::Texture texture;
			};
			
			struct Level
			{
				//the level background texture..
				sp::Texture buffer;
				
				//the level map..
				SPuint8* map;
			};
			
			// draw function to render to an offscreen texture..
			void render(Level* level);
		
			// the binary tree strucutre..
			sp::NBT m_tree;
			
			// the texture atlas..
			TexAtlasInfo m_tex_atlas;
			
			// the tile-table..
			std::map<pm_id_t, TileInfo> m_table;
			
			// the level array..
			std::vector<Level*> m_levels;
			SPindex m_current_level;
	};
}
#endif
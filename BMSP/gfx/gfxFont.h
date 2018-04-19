#pragma once
#include "../managers/ResourceManager.h"

namespace gfx
{
	class gfxFont
	{
	private:
		FT_Face face = nullptr;
		struct TextureCursor
		{
			GLuint texture_id;
			int current_position_x;
			int current_position_y;
			int current_height;
		};
		struct FontSprite
		{
			std::shared_ptr<Sprite> sprite;
			int left, top, advance;
		};
		struct Glyph_Group
		{
			std::map<char, FontSprite> spritemap;
			std::vector<TextureCursor> textures;
		};

		std::map<int, Glyph_Group> glyph_map;
	public:
		gfxFont();
		~gfxFont();
		void LoadFont(std::string font);

		//void setPixelSize(int pixel);
		FontSprite LoadChar(unsigned int ch, int pixel);
	};
}
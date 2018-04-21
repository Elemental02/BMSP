#include "../stdafx.h"
#include "gfxFont.h"
#include "../managers/GlobalManager.h"

const int maxTextureSize = 2048;

gfx::gfxFont::gfxFont()
{
}

gfx::gfxFont::~gfxFont()
{
	if(face != nullptr)
		FT_Done_Face(face);
}

void gfx::gfxFont::LoadFont(std::string font)
{
	auto ft = IgfxGlobal->getFreeTypeLib();
	FT_New_Face(ft, font.c_str(), 0, &face);

}

gfx::gfxFont::FontSprite gfx::gfxFont::LoadChar(unsigned int ch, int pixel)
{
	auto& glyph_group_it = glyph_map.find(pixel);
	if (glyph_group_it == glyph_map.end())
	{
		glyph_map[pixel] = Glyph_Group();
		glyph_group_it = glyph_map.find(pixel);
	}

	auto& glyph_group = glyph_group_it->second;

	auto& sprite_it = glyph_group.spritemap.find(ch);
	bool need_new_texture_cursor = false;
	if (sprite_it == glyph_group.spritemap.end())
	{
		if (glyph_group.textures.empty())
		{
			need_new_texture_cursor = true;
		}
		else
		{
			auto& texture_cursor = glyph_group.textures.back();
			if (texture_cursor.current_position_x + face->glyph->bitmap.width > maxTextureSize)
			{
				texture_cursor.current_position_x = 0;
				texture_cursor.current_position_y += texture_cursor.current_height + 1;
				texture_cursor.current_height = 0;
			}
			if (texture_cursor.current_position_y + face->glyph->bitmap.rows > maxTextureSize)
			{
				need_new_texture_cursor = true;
			}
		}

		if (need_new_texture_cursor)
		{
			glyph_group.textures.push_back(TextureCursor());
			auto& texture_cursor = glyph_group.textures.back();
			glm::u8* buffer = new glm::u8[maxTextureSize * maxTextureSize];
			memset(buffer, 0, maxTextureSize * maxTextureSize);

			glGenTextures(1, &texture_cursor.texture_id);
			glBindTexture(GL_TEXTURE_2D, texture_cursor.texture_id);
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

			glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, maxTextureSize, maxTextureSize, 0, GL_RED, GL_UNSIGNED_BYTE, buffer);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glGenerateMipmap(GL_TEXTURE_2D);
			delete[] buffer;
		}

		FT_Set_Pixel_Sizes(face, 0, pixel);
		FT_Load_Char(face, ch, FT_LOAD_RENDER);

		auto& texture_cursor = glyph_group.textures.back();
		auto glyph = face->glyph;
		glBindTexture(GL_TEXTURE_2D, texture_cursor.texture_id);
		glTexSubImage2D(GL_TEXTURE_2D, 0, texture_cursor.current_position_x, texture_cursor.current_position_y, glyph->bitmap.width, glyph->bitmap.rows, GL_RED, GL_UNSIGNED_BYTE, glyph->bitmap.buffer);
		std::shared_ptr<Sprite> sprite(new Sprite);
		sprite->delete_itself = false;
		sprite->size = glm::vec2(glyph->bitmap.width,glyph->bitmap.rows);

		float uv_left = texture_cursor.current_position_x, uv_right = glyph->bitmap.width;
		uv_left /= maxTextureSize;
		uv_right /= maxTextureSize;
		float uv_up = texture_cursor.current_position_y, uv_down = glyph->bitmap.rows;
		uv_up /= maxTextureSize;
		uv_down /= maxTextureSize;

		sprite->texture_rect = glm::vec4(uv_left, uv_up, uv_right, uv_down);
		sprite->texture_id = texture_cursor.texture_id;
		texture_cursor.current_position_x += glyph->bitmap.width + 1;
		texture_cursor.current_height = texture_cursor.current_height > glyph->bitmap.rows ? texture_cursor.current_height : glyph->bitmap.rows;
		FontSprite font_sprite;
		font_sprite.sprite = sprite;
		font_sprite.left = glyph->bitmap_left;
		font_sprite.top = glyph->bitmap_top;
		font_sprite.advance = glyph->advance.x / 64.0f;

		glyph_group.spritemap[ch] = font_sprite;
	}
	sprite_it = glyph_group.spritemap.find(ch);
	return sprite_it->second;
}

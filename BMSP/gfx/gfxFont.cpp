#include "../stdafx.h"
#include "gfxFont.h"
#include "../managers/GlobalManager.h"

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

void gfx::gfxFont::setPixelSize(int pixel)
{
	FT_Set_Pixel_Sizes(face, 0, pixel);
}

FT_GlyphSlot gfx::gfxFont::LoadChar(unsigned int ch)
{
	FT_Load_Char(face, ch, FT_LOAD_RENDER);
	return face->glyph;
}

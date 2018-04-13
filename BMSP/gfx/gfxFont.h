#pragma once

namespace gfx
{
	class gfxFont
	{
	private:
		FT_Face face = nullptr;
	public:
		gfxFont();
		~gfxFont();
		void LoadFont(std::string font);

		void setPixelSize(int pixel);
		FT_GlyphSlot LoadChar(unsigned int ch);
	};
}
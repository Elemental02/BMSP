#include "../stdafx.h"
#include "Sprite.h"
#include "SpritePackage.h"

void SpritePackage::addSprite(const std::string & name, std::shared_ptr<Sprite> sprite)
{
}

void SpritePackage::addSprites(const std::string & list_path)
{
	float width = size.x;
	float height = size.y;
	if (list_path == "skin1")
	{
		float size_ratio = 800.0f / 640.0f;
		{
			float sizex = 640, sizey = 20;
			float posx = 0, posy = 69;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			sprite_map["top"] = sprite;
		}

		{
			float sizex = 640, sizey = 47;
			float posx = 0, posy = 90;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			sprite_map["bottom"] = sprite;
		}

		for (int i = 0; i < 10; i++)
		{
			float sizex = 12, sizey = 16;
			float posx = 13 * i, posy = 138;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			std::string name = std::to_string(i) + "_s";
			sprite_map[name] = sprite;
		}

		for (int i = 0; i < 10; i++)
		{
			float sizex = 18, sizey = 24;
			float posx = 19 * i + 143, posy = 138;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			std::string name = std::to_string(i) + "_l";
			sprite_map[name] = sprite;
		}

		{
			float sizex = 63, sizey = 24;
			float posx = 333, posy = 138;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			std::string name = "max";
			sprite_map[name] = sprite;
		}
		for (int i = 0; i < 4; i++)
		{
			float sizex = 125, sizey = 32;
			float posx = 0, posy = 163 + 33 * i;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			std::string name = "cool_" + std::to_string(i);
			sprite_map[name] = sprite;
		}


		for (int i = 0; i < 4; i++)
		{
			float sizex = 125, sizey = 32;
			float posx = 126, posy = 163 + 33 * i;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			std::string name = "great_" + std::to_string(i);
			sprite_map[name] = sprite;
		}


		for (int i = 0; i < 4; i++)
		{
			float sizex = 125, sizey = 32;
			float posx = 126 * 2, posy = 163 + 33 * i;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			std::string name = "good_" + std::to_string(i);
			sprite_map[name] = sprite;
		}


		for (int i = 0; i < 4; i++)
		{
			float sizex = 125, sizey = 32;
			float posx = 126 * 3, posy = 163 + 33 * i;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			std::string name = "bad_" + std::to_string(i);
			sprite_map[name] = sprite;
		}


		for (int i = 0; i < 4; i++)
		{
			float sizex = 125, sizey = 32;
			float posx = 126 * 4, posy = 163 + 33 * i;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			std::string name = "miss_" + std::to_string(i);
			sprite_map[name] = sprite;
		}

		{
			float sizex = 125, sizey = 32;
			float posx = 0, posy = 163 + 33 * 4;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			std::string name = "autoplay";
			sprite_map[name] = sprite;
		}

		{
			float sizex = 125, sizey = 32;
			float posx = 126, posy = 163 + 33 * 4;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			std::string name = "replay";
			sprite_map[name] = sprite;
		}

		{
			float sizex = 125, sizey = 32;
			float posx = 126 * 2, posy = 163 + 33 * 4;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			std::string name = "cleared";
			sprite_map[name] = sprite;
		}

		{
			float sizex = 125, sizey = 32;
			float posx = 126 * 3, posy = 163 + 33 * 4;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			std::string name = "failed";
			sprite_map[name] = sprite;
		}
		{
			float sizex = 87, sizey = 103;
			float posx = 384, posy = 353;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			std::string name = "judgement";
			sprite_map[name] = sprite;
		}
		{
			float sizex = 35, sizey = 86;
			float posx = 472, posy = 366;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			std::string name = "judgement_overlay";
			sprite_map[name] = sprite;
		}
		for (int i = 0; i < 6; i++)
		{
			float sizex = 63, sizey = 63;
			float posx = 64 * i + 0, posy = 328;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			std::string name = "effect_" + std::to_string(i);
			sprite_map[name] = sprite;
		}
	}
	else if (list_path == "skin2")
	{
		float size_ratio = 800.0f / 640.0f;

		{
			float sizex = 23, sizey = 412;
			float posx = 0, posy = 0;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			std::string name = "left";
			sprite_map[name] = sprite;
		}

		{
			float sizex = 23, sizey = 412;
			float posx = 24, posy = 0;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			std::string name = "right";
			sprite_map[name] = sprite;
		}

		{
			float sizex = 20, sizey = 30;
			float posx = 48, posy = 31;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			std::string name = "button0_0";
			sprite_map[name] = sprite;
		}

		{
			float sizex = 20, sizey = 30;
			float posx = 86, posy = 31;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			std::string name = "button0_1";
			sprite_map[name] = sprite;
		}
		{
			float sizex = 16, sizey = 30;
			float posx = 69, posy = 31;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			std::string name = "button1_0";
			sprite_map[name] = sprite;
		}
		{
			float sizex = 16, sizey = 30;
			float posx = 107, posy = 31;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			std::string name = "button1_1";
			sprite_map[name] = sprite;
		}
		{
			float sizex = 34, sizey = 30;
			float posx = 48, posy = 62;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			std::string name = "button2_0";
			sprite_map[name] = sprite;
		}
		{
			float sizex = 34, sizey = 30;
			float posx = 83, posy = 62;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			std::string name = "button2_1";
			sprite_map[name] = sprite;
		}
		for (int i = 0; i < 4; i++)
		{
			float sizex = 19, sizey = 5;
			float posx = 48, posy = 97 + 15 * i;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			std::string name = "note0_" + std::to_string(i);
			sprite_map[name] = sprite;
		}
		for (int i = 0; i < 4; i++)
		{
			float sizex = 15, sizey = 5;
			float posx = 88, posy = 97 + 15 * i;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			std::string name = "note1_" + std::to_string(i);
			sprite_map[name] = sprite;
		}
		for (int i = 0; i < 4; i++)
		{
			float sizex = 33, sizey = 5;
			float posx = 104, posy = 97 + 15 * i;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			std::string name = "note2_" + std::to_string(i);
			sprite_map[name] = sprite;
		}
		{
			float sizex = 1, sizey = 382;
			float posx = 141, posy = 0;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			std::string name = "lane";
			sprite_map[name] = sprite;
		}
		{
			float sizex = 1, sizey = 382;
			float posx = 138, posy = 0;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			std::string name = "line1";
			sprite_map[name] = sprite;
		}
		{
			float sizex = 20, sizey = 1;
			float posx = 2, posy = 415;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			std::string name = "line2";
			sprite_map[name] = sprite;
		}
		{
			float sizex = 19, sizey = 377;
			float posx = 210, posy = 0;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			std::string name = "lane0";
			sprite_map[name] = sprite;
		}
		{
			float sizex = 15, sizey = 377;
			float posx = 230, posy = 0;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			std::string name = "lane1";
			sprite_map[name] = sprite;
		}
		{
			float sizex = 33, sizey = 377;
			float posx = 266, posy = 0;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			std::string name = "lane2";
			sprite_map[name] = sprite;
		}
		{
			float sizex = 1, sizey = 1;
			float posx = 0, posy = 0;
			float up = posy, down = sizey, left = posx, right = sizex;
			up /= height;
			down /= height;
			left /= width;
			right /= width;
			glm::vec2 size(::ceil(sizex*size_ratio), ::ceil(sizey*size_ratio));
			std::shared_ptr<Sprite> sprite(new Sprite(texture_id, size, glm::vec4(left, up, right, down), false));
			std::string name = "black";
			sprite_map[name] = sprite;
		}
	}
	else
	{

	}
}

std::shared_ptr<Sprite> SpritePackage::getSprite(const std::string & name)
{
	if (sprite_map.find(name) != sprite_map.end())
		return sprite_map[name];
	return nullptr;
}

SpritePackage::SpritePackage(const glm::uint& texture_id, const glm::vec2& size) : texture_id(texture_id), size(size)
{
}

SpritePackage::~SpritePackage()
{
	glDeleteTextures(1, &texture_id);
}
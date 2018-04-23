#include "../stdafx.h"
#include "gfxSpriteAnimation.h"

bool gfx::gfxSpriteAnimation::Update(std::chrono::milliseconds delta)
{
	current_duration += delta.count();
	if (current_duration > duration)
	{
		current_duration -= duration;
		current_index++;
		if (current_index >= sprite_list.size())
		{
			current_index = 0;
			if (loop > 0)
				loop--;
			else
			{
				current_duration = 0;
				return false;
			}
		}
		setSprite(sprite_list[current_index]);
	}
	return true;
}

void gfx::gfxSpriteAnimation::setList(std::vector<std::shared_ptr<Sprite>> list)
{
	sprite_list = list;
}

void gfx::gfxSpriteAnimation::addLastFrame(std::vector<std::shared_ptr<Sprite>> list)
{
	sprite_list.insert(sprite_list.end(), list.begin(), list.end());
}

void gfx::gfxSpriteAnimation::addLastFrame(std::shared_ptr<Sprite> sprite)
{
	sprite_list.push_back(sprite);
}

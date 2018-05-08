#include "../stdafx.h"
#include "gfxObject.h"
#include "../managers/ResourceManager.h"
#include "gfxSprite9.h"

void gfx::gfxSprite9::Update()
{
}

gfx::gfxSprite9::gfxSprite9()
{
}

void gfx::gfxSprite9::setSprite(std::shared_ptr<Sprite> sprite, glm::vec4 rect)
{
	this->sprite = sprite;
	this->rect = rect;

	for (int i = 0; i < 9; i++)
	{
		sprites[i].delete_itself = false;
		sprites[i].texture_id = sprite->texture_id;
	}

	sprites[0].size = glm::vec2(rect.x, rect.y);
	sprites[1].size = glm::vec2((rect.z - rect.x), rect.y);
	sprites[2].size = glm::vec2(sprite->size.x - rect.z, rect.y);

	sprites[3].size = glm::vec2(rect.x, rect.w - rect.y);
	sprites[4].size = glm::vec2((rect.z - rect.x), rect.w - rect.y);
	sprites[5].size = glm::vec2(sprite->size.x - rect.z, rect.w - rect.y);

	sprites[0].size = glm::vec2(rect.x, sprite->size.y-rect.w);
	sprites[1].size = glm::vec2((rect.z - rect.x), sprite->size.y-rect.w);
	sprites[2].size = glm::vec2(sprite->size.x - rect.z, sprite->size.y - rect.w);
}

gfx::gfxSprite9::~gfxSprite9()
{
}

void gfx::gfxSprite9::Render()
{
	if (!this->sprite)
		return;
}

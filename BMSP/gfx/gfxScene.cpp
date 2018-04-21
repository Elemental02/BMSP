#include "../stdafx.h"
#include "gfxScene.h"

gfx::gfxScene::gfxScene()
{
}

gfx::gfxScene::~gfxScene()
{
}

void gfx::gfxScene::AddUpdatable(std::shared_ptr<gfxUpdatable> updatable)
{
	if (updatable_list.find(updatable) == updatable_list.end())
	{
		updatable_list.insert(updatable);
	}
}

void gfx::gfxScene::RemoveUpdatable(std::shared_ptr<gfxUpdatable> updatable)
{
	if (updatable_list.find(updatable) != updatable_list.end())
	{
		updatable_list.erase(updatable);
	}
}

void gfx::gfxScene::Update(std::chrono::milliseconds delta)
{
	std::vector<std::shared_ptr<gfxUpdatable>> removes;
	for (auto updatable : updatable_list)
	{
		if (!updatable->Update(delta))
			removes.push_back(updatable);
	}
	for (auto remove : removes)
	{
		updatable_list.erase(remove);
	}
}

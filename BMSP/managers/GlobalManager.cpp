#include "../stdafx.h"
#include "GlobalManager.h"

GlobalManager::GlobalManager()
{
	this->resourceManager = std::shared_ptr<ResourceManager>(new ResourceManager);
	this->inputManager = std::shared_ptr<InputManager>(new InputManager);
	this->gfxManager = std::shared_ptr<gfx::gfxGlobal>(new gfx::gfxGlobal);
	this->sfxManager = std::shared_ptr<sfx::sfxGlobal>(new sfx::sfxGlobal);
}

void GlobalManager::Pop_Scene()
{
	if (!scene_stack.empty())
		scene_stack.pop_back();
}

void GlobalManager::Push_Scene(std::shared_ptr<gfx::gfxScene> scene)
{
	scene->Init();
	scene_stack.push_back(scene);
}

void GlobalManager::Update(std::chrono::milliseconds delta)
{
	if (!scene_stack.empty() && scene_stack.back().get() != nullptr)
	{
		scene_stack.back()->Update(delta);
	}
}

void GlobalManager::Render()
{
	if (!scene_stack.empty() && scene_stack.back().get() != nullptr)
	{
		scene_stack.back()->Render();
	}
}

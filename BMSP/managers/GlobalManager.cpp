#include "../stdafx.h"
#include "GlobalManager.h"

GlobalManager::GlobalManager()
{
	this->resourceManager = std::shared_ptr<ResourceManager>(new ResourceManager);
	this->inputManager = std::shared_ptr<InputManager>(new InputManager);
	this->gfxManager = std::shared_ptr<gfx::gfxGlobal>(new gfx::gfxGlobal);
	this->sfxManager = std::shared_ptr<sfx::sfxGlobal>(new sfx::sfxGlobal);
}
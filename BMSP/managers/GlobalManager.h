#pragma once
#include "InputManager.h"
#include "ResourceManager.h"
#include "../gfx/gfxGlobal.h"
#include "../sfx/sfxGlobal.h"

class GlobalManager
{
private:
	GlobalManager();
	std::shared_ptr<ResourceManager> resourceManager;
	std::shared_ptr<InputManager> inputManager;
	std::shared_ptr<gfx::gfxGlobal> gfxManager;
	std::shared_ptr<sfx::sfxGlobal> sfxManager;

public:
	static GlobalManager* Instance()
	{
		static GlobalManager* instance = nullptr;
		if (instance == nullptr)
			instance = new GlobalManager();
		return instance;
	}

	inline std::shared_ptr<ResourceManager> getResourceManager()
	{
		return resourceManager;
	}
	
	inline std::shared_ptr<InputManager> getInputManager()
	{
		return inputManager;
	}
	
	inline std::shared_ptr<gfx::gfxGlobal> getGfxManager()
	{
		return gfxManager;
	}
	
	inline std::shared_ptr<sfx::sfxGlobal> getSfxManager()
	{
		return sfxManager;
	}
};

#define IResourceManager (GlobalManager::Instance()->getResourceManager())
#define IInputManager (GlobalManager::Instance()->getInputManager())
#define IgfxGlobal (GlobalManager::Instance()->getGfxManager())
#define IsfxGlobal (GlobalManager::Instance()->getSfxManager())
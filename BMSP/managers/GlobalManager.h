#pragma once
#include "InputManager.h"
#include "ResourceManager.h"
#include "../gfx/gfxGlobal.h"
#include "../sfx/sfxGlobal.h"

#include "../gfx/gfxScene.h"

class GlobalManager
{
private:
	GlobalManager();
	std::shared_ptr<ResourceManager> resourceManager;
	std::shared_ptr<InputManager> inputManager;
	std::shared_ptr<gfx::gfxGlobal> gfxManager;
	std::shared_ptr<sfx::sfxGlobal> sfxManager;

	std::list<std::shared_ptr<gfx::gfxScene>> scene_stack;

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

	void Pop_Scene();
	void Push_Scene(std::shared_ptr<gfx::gfxScene> scene);
	void Update(std::chrono::milliseconds delta);
	void Render();
};

#define IGlobalManager (GlobalManager::Instance())
#define IResourceManager (GlobalManager::Instance()->getResourceManager())
#define IInputManager (GlobalManager::Instance()->getInputManager())
#define IgfxGlobal (GlobalManager::Instance()->getGfxManager())
#define IsfxGlobal (GlobalManager::Instance()->getSfxManager())
#pragma once
#include "../gfx/gfxScene.h"
#include "../gfx/gfxString.h"

class ListScene : public gfx::gfxScene
{
private:
	enum SceneState { Initializing, List_Loading, List_Playing, Game_Loading };
	SceneState scene_state = Initializing;
	std::shared_ptr<gfx::gfxFont> mainfont;

	gfx::gfxString str_autoplay;
	gfx::gfxString str_autoplay_on;
	gfx::gfxString str_autoplay_off;

	//std::list<gfx::gfxString> str_bms_list;
	std::vector<gfx::gfxString> str_bms_list;
	std::vector<std::string> bms_path_list;
	std::vector<BMS> bms_list;
	
	gfx::gfxString str_list_corsor;

public:
	ListScene();
	virtual ~ListScene();
	virtual void Update(std::chrono::milliseconds delta);
	virtual void Render();

	void Init();
};
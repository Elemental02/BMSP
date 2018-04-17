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

	struct mutex_string
	{
		std::mutex mutex;
		std::string string;

		void setString(std::string str)
		{
			std::lock_guard<std::mutex> lock(mutex);
			string = str;
		}

		std::string getString()
		{
			std::lock_guard<std::mutex> lock(mutex);
			std::string res = string;
			return res;
		}
	};

	std::future<int> init_future;
	mutex_string init_string;

public:
	ListScene();
	virtual ~ListScene();
	virtual void Update(std::chrono::milliseconds delta);
	virtual void Render();

	virtual void Init();
};
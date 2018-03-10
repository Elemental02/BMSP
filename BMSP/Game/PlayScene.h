#pragma once
#include "../BMS/BMSPlayer.h"
#include "../gfx/gfxScene.h"

class PlayScene : public gfx::gfxScene
{
private:
	BMSPlayer bmsPlayer;
	std::map<int, std::shared_ptr<Sound>> sounds;

	std::chrono::system_clock::time_point start_time;
	std::chrono::system_clock::duration prev_time;

	std::chrono::milliseconds processed_time;

	struct SoundPool
	{
		std::list<std::shared_ptr<sfx::sfxSound>> pool;
		std::list<std::shared_ptr<sfx::sfxSound>> playing;
	};
	SoundPool soundpool;
public:
	PlayScene();
	virtual ~PlayScene();
	virtual void Update(std::chrono::milliseconds delta);
	virtual void Render();

	void Init();
};
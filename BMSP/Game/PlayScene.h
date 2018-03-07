#pragma once
#include "../BMS/BMS.h"
#include "../gfx/gfxScene.h"

class PlayScene : public gfx::gfxScene
{
private:
	BMS bms;
public:
	PlayScene();
	virtual ~PlayScene();
	virtual void Update(std::chrono::milliseconds delta);
	virtual void Render();

	void Init();
};
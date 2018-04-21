#pragma once
#include "../BMS/BMSPlayer.h"
#include "../gfx/gfxScene.h"
#include "../gfx/gfxSprite.h"
#include "../sfx/sfxSound.h"
#include "../gfx/gfxPanel.h"

class PlayScene : public gfx::gfxScene
{
private:
	enum SceneState {Initialzing, Loading, Playing, End };
	SceneState scene_state = SceneState::Initialzing;
	BMSPlayer bmsPlayer;
	std::map<int, std::shared_ptr<Sound>> sounds;
	std::map<int, std::shared_ptr<Sprite>> bga_sprites;

	struct SoundPool
	{
		std::list<std::shared_ptr<sfx::sfxSound>> pool;
		std::list<std::shared_ptr<sfx::sfxSound>> playing;
	};
	SoundPool soundpool;

	gfx::gfxPanel node_panel;
	struct NodeSprite
	{
		std::shared_ptr<gfx::gfxSprite> sprite;
		const BMSNode* node;
		int lane_index;
	};
	struct SpritePool
	{
		std::list<NodeSprite> pool;
		std::list<NodeSprite> playing;
	};
	SpritePool node_pool;
	int processed_measure = -1;
	double processed_progress = -1;

	struct LaneInfo
	{
		int lane_value;
		NodeSprite* closest_node;
		int sound_value;
	};
	std::array<LaneInfo, 8> lane_info;

	std::shared_ptr<gfx::gfxSprite> bgaSprite;
	std::shared_ptr<gfx::gfxSprite> bgaLayerSprite;
	
	std::shared_ptr<SpritePackage> sprite_pack1;
	std::shared_ptr<SpritePackage> sprite_pack2;
	
	std::shared_ptr<gfx::gfxSprite> skin_top;
	std::shared_ptr<gfx::gfxSprite> skin_bottom;
	std::shared_ptr<Sprite> nodeSprite;

	float noteSpeed = 550.0f;

	std::string path_to_load;

	void SoundPlay(int value);
public:
	PlayScene();
	virtual ~PlayScene();
	virtual void Update(std::chrono::milliseconds delta);
	virtual void Render();

	virtual void Init();

	void SetBMSPath(std::string path);
};
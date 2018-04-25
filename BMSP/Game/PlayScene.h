#pragma once
#include "../BMS/BMSPlayer.h"
#include "../gfx/gfxScene.h"
#include "../gfx/gfxSprite.h"
#include "../sfx/sfxSound.h"
#include "../gfx/gfxPanel.h"
#include "../gfx/gfxSpriteAnimation.h"
#include "../gfx/gfxSpriteFont.h"
#include "../gfx/gfxVideo.h"

class PlayScene : public gfx::gfxScene
{
private:
	enum SceneState {Initialzing, Playing_Animation, Loading, Playing, End };
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
	gfx::gfxPanel skin_panel;
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
		int sound_value = -1;
		float length;
	};
	std::array<LaneInfo, 8> lane_info;
	int video_value = 0;
	std::shared_ptr<gfx::gfxVideo> bgaVideo;
	std::shared_ptr<gfx::gfxSprite> bgaSprite;
	std::shared_ptr<gfx::gfxSprite> bgaLayerSprite;
	
	std::shared_ptr<SpritePackage> sprite_pack1;
	std::shared_ptr<SpritePackage> sprite_pack2;
	
	std::shared_ptr<gfx::gfxSprite> skin_top;
	std::shared_ptr<gfx::gfxSprite> skin_bottom;
	std::shared_ptr<gfx::gfxSprite> skin_left;
	std::shared_ptr<gfx::gfxSprite> skin_right;

	std::shared_ptr<gfx::gfxSprite> button_sprite[8];
	std::shared_ptr<gfx::gfxSprite> line_sprite[7];

	std::shared_ptr<Sprite> noteAnimation[3][4];
	std::shared_ptr<gfx::gfxSpriteAnimation> judgeAnimation[5];
	std::shared_ptr<gfx::gfxSpriteAnimation> curr_judge;

	std::shared_ptr<gfx::gfxSprite> judgeSkin;
	std::shared_ptr<gfx::gfxSprite> judgeLayout;
	std::shared_ptr<gfx::gfxUpdatableFunc> judge_func;
	int judge_duration = 0;

	std::shared_ptr<gfx::gfxSprite> black_sprite;

	std::shared_ptr<gfx::gfxSprite> lane;
	std::shared_ptr<gfx::gfxSprite> lane_back[8];

	int combo = 0;
	std::shared_ptr<gfx::gfxSprite> combo_max;
	std::shared_ptr<gfx::gfxSpriteFont> combo_sprite;

	float noteSpeed = 550.0f;

	std::future<int> init_future;

	std::string path_to_load;

	bool autoplay = false;

	void SoundPlay(int value);
public:
	PlayScene();
	virtual ~PlayScene();
	virtual void Update(std::chrono::milliseconds delta);
	virtual void Render();

	virtual void Init();

	void SetBMSPath(std::string path);
};
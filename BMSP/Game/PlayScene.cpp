#include "../stdafx.h"
#include "../managers/ResourceManager.h"
#include "../managers/InputManager.h"
#include "../sfx/sfxSound.h"
#include "PlayScene.h"
#include "../BMS/BMSParser.h"
#include <GLFW/glfw3.h>

void PlayScene::SoundPlay(int value)
{
	if (sounds.find(value) != sounds.end()) {
		if (soundpool.pool.empty())
		{
			soundpool.pool.push_back(std::shared_ptr<sfx::sfxSound>(new sfx::sfxSound()));
		}
		auto source = soundpool.pool.back();
		soundpool.pool.pop_back();
		source->setSound(sounds[value]);
		source->Play();
		soundpool.playing.push_back(source);
	}
}

PlayScene::PlayScene(): gfxScene()
{
}

PlayScene::~PlayScene()
{
}

void PlayScene::Update(std::chrono::milliseconds delta)
{
	for (auto sound : soundpool.playing)
	{
		if (!sound->isPlaying())
		{
			soundpool.pool.push_back(sound);
		}
	}
	soundpool.playing.remove_if([](auto it) {return !it->isPlaying(); });

	auto& bms = bmsPlayer.getBMS();
	bmsPlayer.Update();
	auto curr_measure = bmsPlayer.getCurrentMeasure();
	auto curr_progress = bmsPlayer.getCurrentPosition();
	while (processed_measure < curr_measure + 5 && (processed_measure+1)<bms.measures.size())
	{
		processed_measure++;
		auto& measure = bms.measures[processed_measure];
		for (int i = 0; i < 9; i++)
		{
			int channel = i + BMS::CH::P1;
			auto& nodes = measure.nodes.find(channel);
			if (nodes == measure.nodes.end())
				continue;
			for (auto& node : nodes->second)
			{
				if (node_pool.pool.empty())
				{
					node_pool.pool.push_back(NodeSprite{ std::shared_ptr<gfx::gfxSprite>(new gfx::gfxSprite()),nullptr });
					node_pool.pool.back().sprite->setSprite(nodeSprite);
				}
				auto& sprite_node = node_pool.pool.back();
				sprite_node.node = &node;
				sprite_node.sprite->setScale(glm::vec3(30.0f / 35.0f, 1.0f, 1.0f));
				if (channel == BMS::CH::P1)
				{
					sprite_node.sprite->setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
					sprite_node.sprite->setPosition(glm::vec3(75.0f + 30.0f*(channel - BMS::CH::P1), 0.0f, 0.0f));
					sprite_node.lane_index = 1;
				}
				else if (channel == (BMS::CH::P1+1))
				{
					sprite_node.sprite->setColor(glm::vec4(0.0, 0.0, 1.0f, 1.0f));
					sprite_node.sprite->setPosition(glm::vec3(75.0f + 30.0f*(channel - BMS::CH::P1), 0.0f, 0.0f));
					sprite_node.lane_index = 2;
				}
				else if (channel == (BMS::CH::P1+2))
				{
					sprite_node.sprite->setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
					sprite_node.sprite->setPosition(glm::vec3(75.0f + 30.0f*(channel - BMS::CH::P1), 0.0f, 0.0f));
					sprite_node.lane_index = 3;
				}
				else if (channel == (BMS::CH::P1+3))
				{
					sprite_node.sprite->setColor(glm::vec4(0.0, 0.0, 1.0f, 1.0f));
					sprite_node.sprite->setPosition(glm::vec3(75.0f + 30.0f*(channel - BMS::CH::P1), 0.0f, 0.0f));
					sprite_node.lane_index = 4;
				}
				else if (channel == (BMS::CH::P1+4))
				{
					sprite_node.sprite->setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
					sprite_node.sprite->setPosition(glm::vec3(75.0f + 30.0f*(channel - BMS::CH::P1), 0.0f, 0.0f));
					sprite_node.lane_index = 5;
				}
				else if (channel == (BMS::CH::P1+5))
				{
					sprite_node.sprite->setScale(glm::vec3(50.0f / 35.0f, 1.0f, 1.0f));
					sprite_node.sprite->setPosition(glm::vec3(25.0f, 0.0f, 0.0f));
					sprite_node.sprite->setColor(glm::vec4(1.0f, 0.0, 0.0, 1.0f));
					sprite_node.lane_index = 0;
				}
				else if (channel == (BMS::CH::P1+7))
				{
					sprite_node.sprite->setColor(glm::vec4(0.0, 0.0, 1.0f, 1.0f));
					sprite_node.sprite->setPosition(glm::vec3(75.0f + 30.0f*(5), 0.0f, 0.0f));
					sprite_node.lane_index = 6;
				}
				else if (channel == (BMS::CH::P1 + 8))
				{
					sprite_node.sprite->setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
					sprite_node.sprite->setPosition(glm::vec3(75.0f + 30.0f*(6), 0.0f, 0.0f));
					sprite_node.lane_index = 7;
				}
				node_panel.addSprite(sprite_node.sprite.get());
				node_pool.playing.push_back(sprite_node);
				node_pool.pool.pop_back();
			}
		}
	}

	for (auto& sprite_node : node_pool.playing)
	{
		if (sprite_node.node == nullptr)
			continue;
		auto pos = sprite_node.sprite->getPosition();
		double ypos = sprite_node.node->position - curr_progress;
		lane_info[sprite_node.lane_index].closest_node = (lane_info[sprite_node.lane_index].closest_node==nullptr)|| (lane_info[sprite_node.lane_index].closest_node->node == nullptr) || (abs(ypos) < abs(lane_info[sprite_node.lane_index].closest_node->node->position - curr_progress)) ? &(sprite_node) : lane_info[sprite_node.lane_index].closest_node;
		lane_info[sprite_node.lane_index].sound_value = lane_info[sprite_node.lane_index].closest_node->node->value;
		pos.y = 465.0f - (ypos) * 550.0f;
		sprite_node.sprite->setPosition(pos);
		if (ypos < -1.0f)
		{
			sprite_node.node = nullptr;
		}
	}
	
	for (int i = 0; i < 8; i++)
	{
		auto state = InputManager::Instance()->getKeyState(i);
		if (state == KeyState::State_Press)
		{
			if (lane_info[i].sound_value != -1)
				SoundPlay(lane_info[i].sound_value);
			if (lane_info[i].closest_node != nullptr && lane_info[i].closest_node->node != nullptr)
			{
				auto judge = std::abs(bmsPlayer.getCurrentTime().count() - lane_info[i].closest_node->node->a_time.count());
				if (judge < 100)
				{
					lane_info[i].closest_node->node = nullptr;
				}
			}
		}
		lane_info[i].closest_node = nullptr;
	}

	node_pool.playing.remove_if([&](auto& it) {
		if (it.node == nullptr)
		{
			node_pool.pool.push_back(it);
			node_panel.RemoveSprite(it.sprite.get());
			return true;
		}
		return false;
	});

	auto& nodes = bmsPlayer.getUpdated();
	for (auto& node : nodes)
	{
		if (node.first != 1)
			continue;
		SoundPlay(node.second.value);
	}
}

void PlayScene::Render()
{
	node_panel.Render();
	skinSprite->Render();
}

void PlayScene::Init()
{
	std::string filename = "resource/Fellhound/Fellhound (Original Mix)7.bms";  //"resource/k/Concertino in Blue [SP ANOTHER].bme";// "resource/Fellhound/Fellhound (Original Mix)7.bms";
	std::experimental::filesystem::path path(filename);
	BMSParser parser;
	//bms = parser.Parse(filename);
	bmsPlayer.setBMS(parser.Parse(filename));
	for (auto& wav : bmsPlayer.getBMS().wavs)
	{
		auto dir = path.parent_path();
		auto wav_filename = dir.append(wav.second);
		sounds[wav.first] = ResourceManager::Instance()->LoadSound(wav_filename.string());
	}

	for (int i = 0; i < 8; i++)
	{
		lane_info[i].closest_node = 0;
	}
	lane_info[0].lane_value = BMS::CH::P1+5;
	lane_info[1].lane_value = BMS::CH::P1;
	lane_info[2].lane_value = BMS::CH::P1+1;
	lane_info[3].lane_value = BMS::CH::P1+2;
	lane_info[4].lane_value = BMS::CH::P1+3;
	lane_info[5].lane_value = BMS::CH::P1+4;
	lane_info[6].lane_value = BMS::CH::P1+7;
	lane_info[7].lane_value = BMS::CH::P1+8;

	skinSprite = std::shared_ptr<gfx::gfxSprite>(new gfx::gfxSprite);
	skinSprite->setSprite(ResourceManager::Instance()->LoadSprite("resource/ui/skin.png"));
	nodeSprite = ResourceManager::Instance()->LoadSprite("resource/ui/note.png");
}

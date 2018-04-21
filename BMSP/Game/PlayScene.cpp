#include "../stdafx.h"
#include "../managers/GlobalManager.h"
#include "PlayScene.h"
#include "../BMS/BMSParser.h"
#include <GLFW/glfw3.h>

void PlayScene::SoundPlay(int value)
{
	if (sounds.find(value) != sounds.end()) {
		if (sounds[value])
		{
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
}

PlayScene::PlayScene(): gfxScene()
{
}

PlayScene::~PlayScene()
{
	for (auto& it : sounds)
	{
		IResourceManager->UnloadSound(it.second);
	}

	for (auto& it : bga_sprites)
	{
		IResourceManager->UnloadSprite(it.second);
	}
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
	while ((processed_progress - curr_progress)*noteSpeed < 800.0f && (processed_measure + 1) < bms.measures.size())
	{
		processed_measure++;
		processed_progress = bms.measures[processed_measure].position + bms.measures[processed_measure].length;
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
					node_pool.pool.push_back(NodeSprite{ std::shared_ptr<gfx::gfxSprite>(new gfx::gfxSprite),nullptr });
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
				else if (channel == (BMS::CH::P1 + 1))
				{
					sprite_node.sprite->setColor(glm::vec4(0.0, 0.0, 1.0f, 1.0f));
					sprite_node.sprite->setPosition(glm::vec3(75.0f + 30.0f*(channel - BMS::CH::P1), 0.0f, 0.0f));
					sprite_node.lane_index = 2;
				}
				else if (channel == (BMS::CH::P1 + 2))
				{
					sprite_node.sprite->setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
					sprite_node.sprite->setPosition(glm::vec3(75.0f + 30.0f*(channel - BMS::CH::P1), 0.0f, 0.0f));
					sprite_node.lane_index = 3;
				}
				else if (channel == (BMS::CH::P1 + 3))
				{
					sprite_node.sprite->setColor(glm::vec4(0.0, 0.0, 1.0f, 1.0f));
					sprite_node.sprite->setPosition(glm::vec3(75.0f + 30.0f*(channel - BMS::CH::P1), 0.0f, 0.0f));
					sprite_node.lane_index = 4;
				}
				else if (channel == (BMS::CH::P1 + 4))
				{
					sprite_node.sprite->setColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
					sprite_node.sprite->setPosition(glm::vec3(75.0f + 30.0f*(channel - BMS::CH::P1), 0.0f, 0.0f));
					sprite_node.lane_index = 5;
				}
				else if (channel == (BMS::CH::P1 + 5))
				{
					sprite_node.sprite->setScale(glm::vec3(50.0f / 35.0f, 1.0f, 1.0f));
					sprite_node.sprite->setPosition(glm::vec3(25.0f, 0.0f, 0.0f));
					sprite_node.sprite->setColor(glm::vec4(1.0f, 0.0, 0.0, 1.0f));
					sprite_node.lane_index = 0;
				}
				else if (channel == (BMS::CH::P1 + 7))
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

	if (IInputManager->getKeyState(KeyIndex::Up) == KeyState::State_Press)
	{
		noteSpeed += 50;
	}

	if (IInputManager->getKeyState(KeyIndex::Down) == KeyState::State_Press && noteSpeed > 50)
	{
		noteSpeed -= 50;
	}

	for (auto& sprite_node : node_pool.playing)
	{
		if (sprite_node.node == nullptr)
			continue;
		auto pos = sprite_node.sprite->getPosition();
		double ypos = sprite_node.node->position - curr_progress;
		lane_info[sprite_node.lane_index].closest_node = (lane_info[sprite_node.lane_index].closest_node == nullptr) || (lane_info[sprite_node.lane_index].closest_node->node == nullptr) || (abs(ypos) < abs(lane_info[sprite_node.lane_index].closest_node->node->position - curr_progress)) ? &(sprite_node) : lane_info[sprite_node.lane_index].closest_node;
		lane_info[sprite_node.lane_index].sound_value = lane_info[sprite_node.lane_index].closest_node->node->value;
		pos.y = 465.0 - (ypos)* noteSpeed;
		sprite_node.sprite->setPosition(pos);
		if (ypos < -1.0f)
		{
			sprite_node.node = nullptr;
		}
	}

	for (int i = 0; i < 8; i++)
	{
		auto state = IInputManager->getKeyState(i);
		if (state == KeyState::State_Press)
		{
			if (lane_info[i].closest_node != nullptr && lane_info[i].closest_node->node != nullptr)
			{
				auto judge = std::abs(bmsPlayer.getCurrentTime().count() - lane_info[i].closest_node->node->a_time.count());
				if (judge < 100)
				{
					lane_info[i].closest_node->node = nullptr;
				}
				SoundPlay(lane_info[i].sound_value);
			}
			else if (lane_info[i].sound_value != -1)
				SoundPlay(lane_info[i].sound_value);
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
		if (node.first == 1)
			SoundPlay(node.second.value);
		else if (node.first == BMS::CH::BGA)
		{
			auto& sprite = bga_sprites[node.second.value];
			if (sprite != nullptr)
			{
				bgaSprite->setSprite(sprite);
				bgaSprite->setScale(glm::vec3(800.0f / sprite->size.x, 600.0f / sprite->size.y, 0.0f));
			}
		}
		else if (node.first == BMS::CH::BGALayer)
		{
			auto& sprite = bga_sprites[node.second.value];
			if (sprite != nullptr)
			{
				bgaLayerSprite->setSprite(sprite);
				bgaLayerSprite->setScale(glm::vec3(800.0f / sprite->size.x, 600.0f / sprite->size.y, 0.0f));
			}
		}
	}

	if (bmsPlayer.IsComplete()|| IInputManager->getKeyState(KeyIndex::ESC) == KeyState::State_Press)
	{
		for (auto& it : soundpool.playing)
		{
			it->Stop();
		}
		IGlobalManager->Pop_Scene();
	}
}

void PlayScene::Render()
{
	bgaSprite->Render();
	bgaLayerSprite->Render();
	node_panel.Render();
	skin_bottom->Render();
	skin_top->Render();
}

void PlayScene::Init()
{
	std::experimental::filesystem::path path(path_to_load);
	BMSParser parser;
	//[&]() {
		bmsPlayer.setBMS(parser.Parse(path_to_load));
		for (auto& wav : bmsPlayer.getBMS().wavs)
		{
			auto dir = path.parent_path();
			auto wav_filename = dir.string() + "\\" + wav.second;
			auto wavptr = IResourceManager->LoadSound(wav_filename);
			if (!wavptr)
			{
				wav_filename.replace(wav_filename.size() - 3, 3, "mp3");
				wavptr = IResourceManager->LoadSound(wav_filename);
			}
			if (!wavptr)
			{
				wav_filename.replace(wav_filename.size() - 3, 3, "ogg");
				wavptr = IResourceManager->LoadSound(wav_filename);
			}
			if (!wavptr)
				std::cout << "load failed?: " << wav.second <<std::endl;
			sounds[wav.first] = wavptr;
		}

		for (auto& bmp : bmsPlayer.getBMS().bmps)
		{
			auto dir = path.parent_path();
			auto bmp_filename = dir.string() + "\\" + bmp.second;
			auto bmp_ptr = IResourceManager->LoadSprite(bmp_filename);
			if (!bmp_ptr)
			{
				bmp_filename.replace(bmp_filename.size() - 3, 3, "png");
				bmp_ptr = IResourceManager->LoadSprite(bmp_filename);
			}
			if (!bmp_ptr)
			{
				bmp_filename.replace(bmp_filename.size() - 3, 3, "jpg");
				bmp_ptr = IResourceManager->LoadSprite(bmp_filename);
			}
			if (!bmp_ptr)
			{
				bmp_filename.replace(bmp_filename.size() - 3, 4, "jpeg");
				bmp_ptr = IResourceManager->LoadSprite(bmp_filename);
			}
			bga_sprites[bmp.first] = bmp_ptr;
		}
	//};

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

	bgaSprite = std::shared_ptr<gfx::gfxSprite>(new gfx::gfxSprite);
	bgaLayerSprite = std::shared_ptr<gfx::gfxSprite>(new gfx::gfxSprite);
	bgaLayerSprite->setShader("layer");
	nodeSprite = IResourceManager->LoadSprite("resource/ui/note.png");

	sprite_pack1 = IResourceManager->LoadSpritePackage("skin1");
	skin_top = std::shared_ptr<gfx::gfxSprite>(new gfx::gfxSprite);
	skin_top->setSprite(IResourceManager->LoadSprite("resource/ui/skin.png"));
	skin_bottom = std::shared_ptr<gfx::gfxSprite>(new gfx::gfxSprite);
	skin_bottom->setSprite(sprite_pack1->sprite_map["bottom"]);
	skin_bottom->setPosition(glm::vec3(0.0f, 600.0f - skin_bottom->getSprite().size.y, 0.0f));

}

void PlayScene::SetBMSPath(std::string path)
{
	path_to_load = path;
}

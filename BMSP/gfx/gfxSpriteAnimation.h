#pragma once
#include "gfxUpdatable.h"
#include "gfxSprite.h"
namespace gfx
{
	class gfxSpriteAnimation : public gfxSprite, public  gfxUpdatable
	{
	private:
		std::vector<std::shared_ptr<Sprite>> sprite_list;
		int duration = 0;
		int current_index = 0;
		int current_duration = 0;
		int loop = 0;

	public:
		virtual bool Update(std::chrono::milliseconds delta);

		void setList(std::vector<std::shared_ptr<Sprite>> list);
		void addLastFrame(std::vector<std::shared_ptr<Sprite>> list);
		void addLastFrame(std::shared_ptr<Sprite>);

		inline void setLoop(bool loop)
		{
			this->loop = loop;
		}

		inline void Rewind(int loop = 1)
		{
			current_index = 0;
			current_duration = 0;
			setSprite(sprite_list[current_index]);
			this->loop = loop;
		}

		inline void setDuration(int dur)
		{
			duration = dur;
		}
	};
}
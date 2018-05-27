#pragma once
#include "../resource/Sprite.h"
#include "gfxUpdatable.h"
#include "gfxSprite.h"

namespace gfx
{
	class gfxVideo : public gfxSprite, public gfxUpdatable
	{
	private:
		std::shared_ptr<ImageStream> image_stream;
		std::list<std::shared_ptr<Sprite>> sprite_list;
		std::shared_ptr<Sprite> sprite;

		int duration = 0;
		int current_duration = 0;

		glm::vec2 image_size;
		std::future<int> image_stream_future;

		void setVideodata();
		void unload_imagestream();
	public:
		void setVideo(const std::string& path);
		void setVideo(std::shared_ptr<ImageStream> stream);
		virtual bool Update(std::chrono::milliseconds delta);

		inline glm::vec2 getImageSize()
		{
			return image_size;
		}
	};
}
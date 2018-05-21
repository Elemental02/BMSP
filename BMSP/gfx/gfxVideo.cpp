#include "../stdafx.h"
#include "../managers/GlobalManager.h"
#include "gfxVideo.h"


void gfx::gfxVideo::setVideodata()
{
	image_size = glm::vec2(image_stream->codec->width, image_stream->codec->height);
	duration = 1000 / (static_cast<float>(image_stream->codec->framerate.num) / image_stream->codec->framerate.den);
	for (int i = 0; i < image_stream->image_buffer.size(); i++)
	{
		sprite_list.push_back(IResourceManager->LoadSprite(image_stream->image_buffer[i]));
	}
}

void gfx::gfxVideo::unload_imagestream()
{
	image_stream.reset();
}

void gfx::gfxVideo::setVideo(const std::string & path)
{
	image_stream = IResourceManager->OpenImageStream(path);
	if (image_stream)
		setVideodata();
}

void gfx::gfxVideo::setVideo(std::shared_ptr<ImageStream> stream)
{
	if (stream)
	{
		image_stream = stream;
		setVideodata();
	}
}

bool gfx::gfxVideo::Update(std::chrono::milliseconds delta)
{
	current_duration += delta.count();
	if (duration <= current_duration)
	{
		if (sprite_list.empty())
		{
			if(image_stream->is_load_complete == true)
				return false;
			std::cout << "frame delayed" << std::endl;
		}
		else
		{
			current_duration -= duration;
			setSprite(sprite_list.front());
			sprite_list.pop_front();
		}
	
		if (image_stream_future.valid())
		{
			if (image_stream_future.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready)
			{
				int loaded = image_stream_future.get();
				for (int i = 0; i < loaded; i++)
				{
					sprite_list.push_back(IResourceManager->LoadSprite(image_stream->image_buffer[i]));
				}
			}
		}
		else if (sprite_list.size() < (image_stream->image_buffer.size() * 3) / 4)
		{
			image_stream_future = std::async(std::launch::async, [&]() {
				auto stream = image_stream;
				if (stream)
					return IResourceManager->LoadImageFrame(stream, 40);
				return 0;
			});
		}
	}
	return true;
}

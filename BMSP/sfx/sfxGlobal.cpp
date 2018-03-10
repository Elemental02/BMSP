#include "../stdafx.h"
#include <AL/al.h>
#include <AL/alc.h>
#include "../managers/ResourceManager.h"
#include "sfxGlobal.h"

void sfx::sfxGlobal::sfx_loop()
{
	while (quit_loop == false)
	{
		decltype(streaming_objects) copylist;
		streaming_mutex.lock();
		copylist.insert(copylist.end(), streaming_objects.begin(), streaming_objects.end());
		//std::copy(streaming_objects.begin(), streaming_objects.end(), copylist);
		streaming_mutex.unlock();

		for (auto& obj : copylist)
		{
			if (!obj->Update())
			{
				std::lock_guard<std::mutex> g(streaming_mutex);
				streaming_objects.remove(obj);
			}
		}

		using namespace std::chrono_literals;
		std::this_thread::sleep_for(10ms);
		std::this_thread::yield();
	}
}

sfx::sfxGlobal::sfxGlobal()
{
	ALCcontext* alContext;
	auto alDevice = alcOpenDevice(nullptr);

	if (alDevice)
	{
		alContext = alcCreateContext(alDevice, nullptr);
		alcMakeContextCurrent(alContext);
	}

	sfx_loop_thread = std::thread(std::bind(&sfxGlobal::sfx_loop, this));
}

sfx::sfxGlobal::~sfxGlobal()
{
	quit();
}

void sfx::sfxGlobal::quit()
{
	if (quit_loop == false)
	{
		quit_loop = true;
		sfx_loop_thread.join();
	}
	for (auto source : source_pool)
	{
		alDeleteSources(1, &source);
	}
}

ALuint sfx::sfxGlobal::TryGetSource()
{
	std::lock_guard<std::mutex> guard(source_pool_mutex);
	ALuint source;
	if (source_pool.empty())
	{
		alGenSources(1, &source);
		return source;
	}
	else {
		source = *source_pool.begin();
		source_pool.pop_front();
	}
	return source;
}

void sfx::sfxGlobal::ReleaseSource(ALuint source)
{
	std::lock_guard<std::mutex> guard(source_pool_mutex);
	int processed;
	unsigned int tempbuf[20];

	alGetSourcei(source, AL_BUFFERS_QUEUED, &processed);
	while (processed != 0)
	{
		if (processed > 20)
			processed = 20;
		alSourceUnqueueBuffers(source, processed, tempbuf);
		alGetSourcei(source, AL_BUFFERS_QUEUED, &processed);
	}
	auto err = alGetError();
	assert(err == AL_NO_ERROR && "setSound");
	source_pool.push_back(source);
}

void sfx::sfxGlobal::ApplyStreamingObject(std::shared_ptr<sfxObject> obj)
{
	std::lock_guard<std::mutex> guard(streaming_mutex);
	streaming_objects.push_back(obj);
}

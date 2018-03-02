#include "../stdafx.h"

#include "../managers/ResourceManager.h"
#include "sfxGlobal.h"
#include "sfxSound.h"

void sfx::sfxSound::setSource()
{
	if (sound == nullptr)
		return;
	if (sourceId != 0)
	{
		Stop();
		sfxGlobal::Instance()->ReleaseSource(sourceId);
	}
	sourceId = sfxGlobal::Instance()->TryGetSource();
}

sfx::sfxSound::~sfxSound()
{
	if (sourceId != 0)
	{
		Stop();
		sfxGlobal::Instance()->ReleaseSource(sourceId);
	}
	sound.reset();
}

void sfx::sfxSound::setSound(std::shared_ptr<Sound> sound)
{
	this->sound = sound;
	if (sourceId == 0)
		setSource();
	alSourceQueueBuffers(sourceId, sound->buffers.size(), sound->buffers.data());
}

void sfx::sfxSound::Play()
{
	if (sound == nullptr)
		return;
	alSourcePlay(sourceId);
	auto err = alGetError();
	if (!sound->is_load_complete)
	{
		std::shared_ptr<sfxObject> streaming_obj(new StreamingObj(this));
		sfxGlobal::Instance()->ApplyStreamingObject(streaming_obj);
	}
}

void sfx::sfxSound::Pause()
{
	if (sourceId == 0)
		return;
	alSourcePause(sourceId);
}

void sfx::sfxSound::Stop()
{
	if (sourceId == 0)
		return;
	alSourceStop(sourceId);
}

bool sfx::sfxSound::Update()
{
	if (sourceId == 0)
		return false;
	if (sound->is_load_complete)
	{
		sfxGlobal::Instance();
		return false;
	}
	int curr_state = 0;
	alGetSourcei(sourceId, AL_SOURCE_STATE, &curr_state);
	if (curr_state == AL_PLAYING)
	{
		int processed, queued;
		alGetSourcei(sourceId, AL_BUFFERS_PROCESSED, &processed);
		alGetSourcei(sourceId, AL_BUFFERS_QUEUED, &queued);
		if (queued - processed < 5)
		{
			ResourceManager::Instance()->LoadSoundFrame(sound);
			alSourceQueueBuffers(sourceId, sound->buffers.size() - queued, sound->buffers.data() + queued);
		}
	}
	else
	{
		return false;
	}
	return true;
}

sfx::sfxSound::StreamingObj::StreamingObj(sfxSound * sound) :soundObj(sound)
{
}

bool sfx::sfxSound::StreamingObj::Update()
{
	if (soundObj != nullptr)
		return soundObj->Update();
	return false;
}

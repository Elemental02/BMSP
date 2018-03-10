#include "../stdafx.h"

#include "../managers/ResourceManager.h"
#include "sfxSound.h"

void sfx::sfxSound::setSource()
{
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
	if (sourceId == 0 || this->sound != nullptr)
		setSource();
	this->sound = sound;
	int queued;
	alGetSourcei(sourceId, AL_BUFFERS_QUEUED, &queued);
	assert(queued==0);
	alSourceQueueBuffers(sourceId, sound->buffers.size(), sound->buffers.data());
	auto err = alGetError();
	assert(err == AL_NO_ERROR && "setSound");
}

void sfx::sfxSound::Play()
{
	if (sound == nullptr)
		return;
	alSourcePlay(sourceId);
	assert(alGetError() == AL_NO_ERROR && "alPlay");
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
	assert(alGetError() == AL_NO_ERROR);
}

void sfx::sfxSound::Stop()
{
	if (sourceId == 0)
		return;
	alSourceStop(sourceId);
	assert(alGetError() == AL_NO_ERROR);
}

bool sfx::sfxSound::Update()
{
	if (sourceId == 0)
		return false;
	if (sound->is_load_complete)
	{
		//sfxGlobal::Instance();
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
			assert(alGetError() == AL_NO_ERROR && "alsourcequeuebuffer");
		}
	}
	else
	{
		return false;
	}
	return true;
}

bool sfx::sfxSound::isPlaying()
{
	int curr_state = 0;
	alGetSourcei(sourceId, AL_SOURCE_STATE, &curr_state);
	return (curr_state == AL_PLAYING);
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

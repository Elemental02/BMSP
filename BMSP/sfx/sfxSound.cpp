#include "../stdafx.h"

#include "../managers/GlobalManager.h"
#include "sfxSound.h"

void sfx::sfxSound::setSource()
{
	if (sourceId != 0)
	{
		Stop();
		IsfxGlobal->ReleaseSource(sourceId);
	}
	sourceId = IsfxGlobal->TryGetSource();
	this->sound.reset();
}

sfx::sfxSound::~sfxSound()
{
	if (sourceId != 0)
	{
		Stop();
		IsfxGlobal->ReleaseSource(sourceId);
	}
}

void sfx::sfxSound::setSound(std::shared_ptr<Sound> sound)
{
	if (sourceId == 0 || this->sound != nullptr)
		setSource();
	this->sound.reset();
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
		if(streaming_obj)
			streaming_obj->reset();
		streaming_obj = std::shared_ptr<StreamingObj>(new StreamingObj(this));
		IsfxGlobal->ApplyStreamingObject(streaming_obj);
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
	if(streaming_obj)
		streaming_obj->reset();
	alSourceStop(sourceId);
	assert(alGetError() == AL_NO_ERROR);
}

bool sfx::sfxSound::Update()
{
	if (sourceId == 0)
		return false;
	if (sound->is_load_complete)
	{
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
			IResourceManager->LoadSoundFrame(sound);
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
	std::lock_guard<std::mutex> guard(mutex);
	if (soundObj != nullptr)
		return soundObj->Update();
	return false;
}

void sfx::sfxSound::StreamingObj::reset()
{
	std::lock_guard<std::mutex> guard(mutex);
	soundObj = nullptr;
}

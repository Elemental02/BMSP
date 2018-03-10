#pragma once
#include "sfxGlobal.h"

namespace sfx
{
	class sfxSound
	{
	private:
		class StreamingObj : public sfxObject
		{
			sfxSound * soundObj;
		public:
			StreamingObj(sfxSound* sound);
			virtual bool Update();
		};
		std::shared_ptr<Sound> sound;
		ALuint sourceId;
		void setSource();
	public:
		~sfxSound();
		void setSound(std::shared_ptr<Sound> sound);
		void Play();
		void Pause();
		void Stop();

		bool Update();

		bool isPlaying();
	};
}
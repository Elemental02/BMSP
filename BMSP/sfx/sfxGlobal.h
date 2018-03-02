#pragma once
namespace sfx
{
	class sfxObject
	{
	public:
		virtual bool Update() = 0;
	};

	class sfxGlobal
	{
	private:
		std::list<ALuint> source_pool;
		std::list<std::shared_ptr<sfxObject>> streaming_objects;
		std::mutex streaming_mutex;

		bool quit_loop;
		std::thread sfx_loop_thread;
		void sfx_loop();
		sfxGlobal();
	public:
		~sfxGlobal();
		static sfxGlobal* Instance()
		{
			static sfxGlobal* instance = nullptr;
			if (instance == nullptr)
				instance = new sfxGlobal();
			return instance;
		}
		void quit();

		ALuint TryGetSource();
		void ReleaseSource(ALuint source);

		void ApplyStreamingObject(std::shared_ptr<sfxObject> obj);
	};
}
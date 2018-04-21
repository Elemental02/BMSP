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
		std::mutex source_pool_mutex;
		std::list<std::shared_ptr<sfxObject>> streaming_objects;
		std::mutex streaming_mutex;

		bool quit_loop;
		std::thread sfx_loop_thread;
		void sfx_loop();
	public:
		sfxGlobal();
		~sfxGlobal();

		void start();
		void quit();

		ALuint TryGetSource();
		void ReleaseSource(ALuint source);

		void ApplyStreamingObject(std::shared_ptr<sfxObject> obj);
	};
}
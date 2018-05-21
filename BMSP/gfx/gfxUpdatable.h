#pragma once
namespace gfx
{
	class gfxUpdatable
	{
	public:
		virtual bool Update(std::chrono::milliseconds delta) = 0;
	};

	class gfxUpdatableFunc : public gfxUpdatable
	{
	private:
		int elasped_time = 0;
		std::function<bool(std::chrono::milliseconds)> function;
	public:
		gfxUpdatableFunc();
		gfxUpdatableFunc(std::function<bool(std::chrono::milliseconds)> func);

		inline void setFunc(std::function<bool(std::chrono::milliseconds)> func)
		{
			function = func;
		}
		inline int getElasped()
		{
			return elasped_time;
		}

		virtual bool Update(std::chrono::milliseconds delta);
	};
}
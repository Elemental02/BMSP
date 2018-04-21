#pragma once
#include "gfxUpdatable.h"
namespace gfx
{
	class gfxScene
	{
		std::set<std::shared_ptr<gfxUpdatable>> updatable_list;
	public:
		gfxScene();
		virtual ~gfxScene();
		void AddUpdatable(std::shared_ptr<gfxUpdatable> upatable);
		void RemoveUpdatable(std::shared_ptr<gfxUpdatable> upatable);
		virtual void Update(std::chrono::milliseconds delta);
		virtual void Render() = 0;
		virtual void Init() = 0;
	};
};
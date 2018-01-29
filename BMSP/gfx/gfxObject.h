#pragma once
namespace gfx
{
	class gfxObject
	{
	private:
		glm::vec3 position = { 0.0f,0.0f,0.0f };
		glm::quat rotation = { 0.0f,0.0f,0.0f, 0.0f };
		glm::vec3 scale = { 1.0f,1.0f,1.0f };
	public:
		gfxObject();
		virtual ~gfxObject();
		virtual glm::mat4 getTransform();
		virtual void Render() = 0;

		inline void setPosition(glm::vec3 pos)
		{
			position = pos;
		}
	};
};
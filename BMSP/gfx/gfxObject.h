#pragma once
namespace gfx
{
	class gfxObject
	{
	private:
		glm::vec3 position = { 0.0f,0.0f,0.0f };
		glm::quat rotation = { 0.0f,0.0f,0.0f, 0.0f };
		glm::vec3 scale = { 1.0f,1.0f,1.0f };
		glm::mat4 mat;

		bool is_dirty = true;
	public:
		gfxObject();
		virtual ~gfxObject();
		virtual glm::mat4 getTransform();
		virtual void Render() = 0;
		
		inline glm::vec3 getPosition()
		{
			return position;
		}

		inline void setPosition(glm::vec3 pos)
		{
			position = pos;
			is_dirty = true;
		}

		inline void setScale(glm::vec3 scale)
		{
			this->scale = scale;
			is_dirty = true;
		}

		inline void setRotation(glm::quat rot)
		{
			rotation = rot;
			is_dirty = true;
		}
	};
};
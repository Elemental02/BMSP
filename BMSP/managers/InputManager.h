#pragma once

enum KeyState
{
	State_Up, State_Press, State_Down, State_Release
};

struct GLFWwindow;

class InputManager
{
private:
	InputManager() {}

	GLFWwindow* window;
	std::vector<int> keys_to_update;
	std::map<int, int> prev_state;
	std::map<int, int> curr_state;
public:
	static InputManager* Instance()
	{
		static InputManager* instance = nullptr;
		if (instance == nullptr)
			instance = new InputManager();
		return instance;
	}

	void Init(GLFWwindow* window);
	void Update();
	KeyState getKeyState(int key);
};
#include "../stdafx.h"
#include <GLFW/glfw3.h>
#include "InputManager.h"

void InputManager::Init(GLFWwindow * window)
{
	this->window = window;
	keys_to_update.push_back(GLFW_KEY_A);
	keys_to_update.push_back(GLFW_KEY_S);
	keys_to_update.push_back(GLFW_KEY_D);
	keys_to_update.push_back(GLFW_KEY_F);
	keys_to_update.push_back(GLFW_KEY_J);
	keys_to_update.push_back(GLFW_KEY_K);
	keys_to_update.push_back(GLFW_KEY_L);
	keys_to_update.push_back(GLFW_KEY_SEMICOLON);
}

void InputManager::Update()
{
	for (auto key : keys_to_update)
	{
		prev_state[key] = curr_state[key];
		curr_state[key] = glfwGetKey(window, key);
	}
	//std::cout << glfwGetKey(window, GLFW_KEY_SPACE) << std::endl;
}

KeyState InputManager::getKeyState(int key_index)
{
	/*if (prev_state.find(key) != prev_state.end() && curr_state.find(key) != curr_state.end())
	{
		if (prev_state[key] != curr_state[key])
		{
			return curr_state[key] == GLFW_PRESS ? KeyState::State_Press : KeyState::State_Release;
		}
		else
		{
			return curr_state[key] == GLFW_PRESS ? KeyState::State_Down : KeyState::State_Up;
		}
	}*/
	int key = keys_to_update[key_index];
	if (prev_state[key] != curr_state[key])
	{
		return curr_state[key] == GLFW_PRESS ? KeyState::State_Press : KeyState::State_Release;
	}
	else
	{
		return curr_state[key] == GLFW_PRESS ? KeyState::State_Down : KeyState::State_Up;
	}
	return KeyState::State_Up;
}

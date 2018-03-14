#pragma once
#include "BMS.h"

class BMSPlayer
{
private:
	BMS bms;
	int current_measure = 0;
	double current_bpm;
	double current_process;

	std::chrono::system_clock::time_point start_time;
	std::chrono::system_clock::time_point prev_time;

	std::chrono::milliseconds play_time;

	std::list<std::pair<int,BMSNode>> updated_nodes;

	bool isPlaying = true;
public:
	void setBMS(BMS bms)
	{
		this->bms = bms;
		current_bpm = bms.bpm;
		current_process = 0.0;
	}
	
	inline double getCurrentPosition()
	{
		return current_process;
	}

	inline auto getCurrentTime()
	{
		return play_time;
	}

	inline int getCurrentMeasure()
	{
		return current_measure;
	}

	inline const BMS& getBMS()
	{
		return bms;
	}

	void Update();
	
	inline const decltype(updated_nodes)& getUpdated()
	{
		return updated_nodes;
	}
};
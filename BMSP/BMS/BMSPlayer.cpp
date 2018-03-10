#include "../stdafx.h"
#include "BMSPlayer.h"

double _4_minute_to_millisecond = 4 * 60 * 1000;

void BMSPlayer::Update(std::chrono::milliseconds delta)
{
	if (!isPlaying)
		return;
	if (start_time == std::chrono::system_clock::time_point())
	{
		start_time = std::chrono::system_clock::now();
	}
	auto curr_time = std::chrono::system_clock::now();
	auto played_time = std::chrono::duration_cast<std::chrono::milliseconds>(curr_time - start_time);
	updated_nodes.clear();

	if (current_bpm == 0)
	{
		return;
	}

	bool loop = false;
	do
	{
		BMSMeasure& measure = bms.measures[current_measure];
		for (auto& channel : measure.nodes)
		{
			for (auto& node : channel.second)
			{
				node.a_time = node.a_time ? node.a_time : node.position / (current_bpm / _4_minute_to_millisecond);
				if (node.a_time <= played_time.count() && node.value != 0)
				{
					//add node with channel number
					updated_nodes.push_back(std::pair<int, BMSNode>(channel.first, node));
					node.value = 0;
				}
			}
		}
		if ((played_time.count()) / (_4_minute_to_millisecond / current_bpm) >= measure.length + measure.position)
		{
			current_measure++;
			std::cout << "measure: " << current_measure << std::endl;
			if (current_measure >= bms.measures.size())
			{
				isPlaying = false;
				return;
			}
			loop = true;
		}
		else
		{
			loop = false;
		}
	} while (loop);
}

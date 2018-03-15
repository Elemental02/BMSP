#include "../stdafx.h"
#include "BMSPlayer.h"

void BMSPlayer::Update()
{
	if (!isPlaying)
		return;
	if (start_time == std::chrono::system_clock::time_point())
	{
		start_time = std::chrono::system_clock::now();
	}
	if (prev_time == std::chrono::system_clock::time_point())
	{
		prev_time = std::chrono::system_clock::now();
	}
	auto curr_time = std::chrono::system_clock::now();
	auto prev_played_time = std::chrono::duration_cast<std::chrono::milliseconds>(prev_time - start_time);
	play_time = std::chrono::duration_cast<std::chrono::milliseconds>(curr_time - start_time);
	updated_nodes.clear();

	bool loop = false;
	BMSNode* stop_node = nullptr;
	do
	{
		BMSMeasure& measure = bms.measures[current_measure];
		for (auto& channel : measure.nodes)
		{
			for (auto& node : channel.second)
			{
				if (node.a_time >= prev_played_time && node.a_time < play_time && node.value != 0)
				{
					if (channel.first == BMS::CH::BPM || channel.first == BMS::CH::extendBPM)
					{
						double node_bpm = channel.first == BMS::CH::BPM ? static_cast<double>(node.value) : bms.bpms[node.value];
						current_bpm = bpm_node ? (bpm_node->position < node.position ? node_bpm : current_bpm) : node_bpm;
						bpm_node = bpm_node ? (bpm_node->position < node.position ? &node : bpm_node) : &node;
					}
					else if (channel.first == BMS::CH::Stop)
					{
						stop_node = stop_node ? (stop_node->position < node.position ? &node : stop_node) : &node;
					}
					else
					{
						updated_nodes.push_back(std::pair<int, BMSNode>(channel.first, node));
					}
				}
			}
		}
		
		if (play_time >= measure.during_time)
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

	if (stop_node && (stop_node->a_time + std::chrono::milliseconds(stop_node->value) > play_time))
		current_process = stop_node->position;
	else if (bpm_node)
		current_process = bpm_node->position + (play_time - bpm_node->a_time).count()*current_bpm / BMS::_4_minute_to_millisecond;
	else
		current_process = (play_time).count()*current_bpm / BMS::_4_minute_to_millisecond;
	prev_time = curr_time;
}

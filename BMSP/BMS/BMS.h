#pragma once

#include "BMSNode.h"

/*struct BMSMetadata
{
	std::string title;
	std::string subtitle;
	std::string artist;
	std::string subartist;
	std::string maker;
	std::string genre;
	std::string comment;
};*/

class BMS
{
public:
	double bpm = 130.0f;
	int player;
	int rank;
	int lnType = 1;
	std::map<int, float> bpms;
	std::set<int> ln_objs;
	std::vector<BMSMeasure> measures;

	std::map<int, std::string> wavs;
	std::map<int, std::string> bmps;
	std::map<std::string, std::string> metadata;
};
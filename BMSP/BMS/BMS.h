#pragma once

class BMSMeasure;

class BMS
{
public:
	float bpm = 130.0f;
	int player;
	int rank;
	std::map<int, float> BPMs;
	std::vector<BMSMeasure> measures;
	std::string title;
	std::string subtitle;
	std::string artist;
	std::string subartist;
	std::string maker;
	std::string genre;
	std::string comment;
};
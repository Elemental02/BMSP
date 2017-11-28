#pragma once

class BMSMeasure;

class BMS
{
public:
	double bpm = 130.0f;
	int player;
	int rank;
	int lnType = 1;
	std::map<int, float> BPMs;
	std::set<int> lnObjs;
	std::vector<BMSMeasure> measures;
	std::string title;
	std::string subtitle;
	std::string artist;
	std::string subartist;
	std::string maker;
	std::string genre;
	std::string comment;
};
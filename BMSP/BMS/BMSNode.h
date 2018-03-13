#pragma once
class BMSNode //³ëÆ®
{
public:
	int value = 0;
	double length = 0;	//·Õ³ò¿ë
	std::chrono::milliseconds a_time;
	double position = 0;
};

class BMSMeasure //¸¶µð
{
public:
	double length = 1.0;
	double position = 0;
	//double bpm = 0.0f;
	std::chrono::milliseconds time;
	std::chrono::milliseconds during_time;
	std::map<int, std::list<BMSNode>> nodes;
};
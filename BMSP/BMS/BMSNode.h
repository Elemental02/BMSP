#pragma once
class BMSNode //³ëÆ®
{
public:
	int value = 0;
	double length = 0;	//·Õ³ò¿ë
	double a_time = 0;
	double position = 0;
};

class BMSMeasure //¸¶µð
{
public:
	double length = 1.0;
	double position = 0;
	double bpm = 0.0f;
	std::map<int, std::list<BMSNode>> nodes;
};
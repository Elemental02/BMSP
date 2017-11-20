#pragma once
class BMSNode
{
public:
	int value = 0;
	uint32_t length = 0;
	uint32_t a_time = 0;
	uint32_t position = 0;
};

class BMSMeasure
{
public:
	uint32_t length = 10000;
	std::map<int, std::list<BMSNode>> nodes;
};
#pragma once
class BMSNode //��Ʈ
{
public:
	int value = 0;
	double length = 0;	//�ճ��
	std::chrono::milliseconds a_time;
	double position = 0;
};

class BMSMeasure //����
{
public:
	double length = 1.0;
	double position = 0;
	//double bpm = 0.0f;
	std::chrono::milliseconds time;
	std::chrono::milliseconds during_time;
	std::map<int, std::list<BMSNode>> nodes;
};
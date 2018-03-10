#pragma once
class BMSNode //��Ʈ
{
public:
	int value = 0;
	double length = 0;	//�ճ��
	double a_time = 0;
	double position = 0;
};

class BMSMeasure //����
{
public:
	double length = 1.0;
	double position = 0;
	double bpm = 0.0f;
	std::map<int, std::list<BMSNode>> nodes;
};
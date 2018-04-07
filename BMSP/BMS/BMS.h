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

	static constexpr int parseValue(const char* value)
	{
		int val = 0;
		while (*value != 0)
		{
			val *= 36;
			if (*value >= 'a' && *value <= 'z')
				val += *value - 'a' + 10;
			else if (*value >= 'A' && *value <= 'Z')
				val += *value - 'A' + 10;
			else
				val += *value - '0';
			value += 1;
		}
		return val;
	};

	struct CH
	{
		static const int P1;// = BMSParser::parseValue("11");
		static const int P2;// = BMSParser::parseValue("21");

		static const int P1L;// = BMSParser::parseValue("51");
		static const int P2L;// = BMSParser::parseValue("61");

		static const int BGM;// = BMSParser::parseValue("01");
		static const int Length;// = BMSParser::parseValue("02");
		static const int BPM;// = BMSParser::parseValue("03");
		static const int BGA;// = BMSParser::parseValue("04");
		static const int PoorBGA;// = BMSParser::parseValue("06");
		static const int BGALayer;// = BMSParser::parseValue("07");
		static const int extendBPM;// = BMSParser::parseValue("08");
		static const int Stop;// = BMSParser::parseValue("09");
	};

	static const double _4_minute_to_millisecond;
};
#pragma once

class BMSParser
{
public:
	BMS Parse(std::string filename, bool metadata_only = false);
	//BMSMetadata ParseMeatdata(std::string filename);
	//const double _4_minute_to_millisecond = 4 * 60 * 1000;
private:
	
};
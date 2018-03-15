#include "../stdafx.h"
#include "BMS.h"

//static values
const int BMS::CH::P1 = 11;// BMS::parseValue("11");
const int BMS::CH::P2 = 21;// BMS::parseValue("21");

const int BMS::CH::P1L = BMS::parseValue("51");
const int BMS::CH::P2L = BMS::parseValue("61");

const int BMS::CH::BGM = BMS::parseValue("01");
const int BMS::CH::Length = BMS::parseValue("02");
const int BMS::CH::BPM = BMS::parseValue("03");
const int BMS::CH::BGA = BMS::parseValue("04");
const int BMS::CH::PoorBGA = BMS::parseValue("06");
const int BMS::CH::BGALayer = BMS::parseValue("07");
const int BMS::CH::extendBPM = BMS::parseValue("08");
const int BMS::CH::Stop = BMS::parseValue("09");

const double BMS::_4_minute_to_millisecond = 4.0 * 60.0 * 1000.0;
#include "../stdafx.h"
#include "BMSParser.h"
#include "BMS.h"
#include "BMSNode.h"
/* header list
#PLAYER
#RANK
#DEFEXRANK
#EXRANKxx
#TOTAL
#VOLWAV
#STAGEFILE
#BANNER
#BACKBMP
#CHARFILE
#PLAYLEVEL
#DIFFICULTY
#TITLE
#SUBTITLE
#ARTIST
#SUBARTIST
#MAKER
#GENRE
#COMMENT
#TEXTxx
#SONGxx landmine
#PATH_WAV
#BPM
#BPMxx #EXBPMxx #BASEBPM #STOPxx #STP
#LNTYPE 1
#LNTYPE 2
#LNOBJ
#OCT/FP #OPTION #CHANGEOPTIONxx
#WAVxx
#WAVCMD
#EXWAVxx
#CDDA
#MIDIFILE
#BMPxx
#EXBMPxx
#BGAxx
#@BGAxx #POORBGA #SWBGAxx #ARGBxx #VIDEOFILE #VIDEOf/s #VIDEOCOLORS #VIDEODLY #MOVIE #SEEKxx #ExtChr MATERIALS #MATERIALSWAV #MATERIALSBMP #DIVIDEPROP
#CHARSET
%URL %EMAIL
*/

int constexpr parseValue(const char* value)
{
	int val = 0;
	while(*value != 0)
	{
		val *= 64;
		if (*value >= 'a' && *value <= 'z')
			val += *value - 'a' + 10;
		else if (*value >= 'A' && *value <= 'Z')
			val += *value - 'A' + 10 + 'z' - 'a';
		else
			val += *value - '0';
		value += 1;
	}
	return val;
}
namespace CH
{
	const int P1 = parseValue("11");
	const int P2 = parseValue("21");

	const int BGM = parseValue("01");
	const int BPM = parseValue("03");
	const int BGA = parseValue("04");
	const int extendBPM = parseValue("08");
};

void BMSParser::Parse()
{
	std::string line = "#00012:000102030405060708090a0b0c0d0e0f0g0h0i0j0k0l0m0n0o0p0q0r0s0t0u0v0w0x0y0z0A0B0C0D0E0F0G0H0I0J0K0L0M0N0O0P0Q0R0S0T0U0V0W0X0Y0Z		; this is test node";
	std::string line2 = "#PLAYER dfkdufj <dkfjdjfkdkfj>  ; this is test";
	BMS bms;
	std::smatch match;
	if (std::regex_match(line, match, std::regex("^#([a-zA-Z][a-zA-Z0-9]*)[ \\t]*(.*)(?:[;(?://)].*)?"))) // header
	{
		// name value
		std::string name = match[1];
		std::string value = match[2];
		std::transform(name.begin(), name.end(), name.begin(), ::toupper);

		if (name == "BPM")
		{
			bms.bpm = std::stof(value);
		}
		else if (name == "RANK")
		{

		}
		else if (name == "PLAYER")
		{
			bms.player = std::stoi(value);
		}
		else if(name == "RANK")
		{
			bms.rank = std::stoi(value);
		}
		else if (name == "DEFEXRANK")
		{
		}
		else if (name == "TOTAL")
		{
		}
		else if (name == "VOLWAV")
		{
		}
		else if (name == "STAGEFILE")
		{
		}
		else if (name == "BANNER")
		{
		}
		else if (name == "BACKBMP")
		{
		}
		else if (name == "CHARFILE")
		{
		}
		else if (name == "PLAYLEVEL")
		{
		}
		else if (name == "DIFFICULTY")
		{
		}
		/*
		else if (name == "TITLE")
		{
			bms.title = value;
		}
		else if (name == "SUBTITLE")
		{
			bms.subtitle = value;
		}
		else if (name == "ARTIST")
		{
			bms.artist = value;
		}
		else if (name == "SUBARTIST")
		{
			bms.subartist = value;
		}
		else if (name == "MAKER")
		{
			bms.maker = value;
		}
		else if (name == "GENRE")
		{
			bms.genre = value;
		}
		else if (name == "COMMENT")
		{
			bms.comment = value;
		}*/
	}
	else if (std::regex_match(line, match, std::regex("^#([0-9][0-9][0-9])([0-9a-zA-Z][0-9a-zA-Z]):((?:[0-9a-zA-Z][0-9a-zA-Z])*).*")))
	{
		// xxxCH:00-ZZ
		std::string measure = match[1];
		std::string channel = match[2];
		
		unsigned int imeasure = std::stoi(measure);
		int ichannel = std::stoi(channel);
		if(bms.measures.size() <= imeasure)
			bms.measures.resize(imeasure + 1);
		if (bms.measures[imeasure].nodes.find(ichannel) == bms.measures[imeasure].nodes.end())
			bms.measures[imeasure].nodes[ichannel] = std::list<BMSNode>();

		std::string nodes = match[3];
		int len = nodes.length() / 2;
		auto& channels = bms.measures[imeasure].nodes[ichannel];
		if (channels.size() < len)
			channels.resize(len);
		int i = 0;
		for (auto& it : channels)
		{
			std::string val = nodes.substr(i * 2, 2);
			int value = parseValue(val.c_str());
			i += 1;
			if(value != 0)
				it.value = value;
		}
	}

	for (auto& measure : bms.measures)
	{
		for (auto& channels : measure.nodes)
		{
			int size = channels.second.size();
			int count = 0;
			for (auto& node : channels.second)
			{
				node.position = count / size;
				count++;
			}
			channels.second.remove_if([](BMSNode& i) { return i.value == 0; });
		}
	}
}
/*
void main()
{
	BMSParser bms;
	bms.Parse();
}*/
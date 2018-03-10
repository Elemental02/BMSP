#include "../stdafx.h"
#include "BMS.h"
#include "BMSNode.h"
#include "BMSParser.h"
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

	const int P1L = parseValue("51");
	const int P2L = parseValue("61");

	const int BGM = parseValue("01");
	const int Length = parseValue("02");
	const int BPM = parseValue("03");
	const int BGA = parseValue("04");
	const int extendBPM = parseValue("08");
};

BMS BMSParser::Parse(std::string filename, bool metadata_only)
{
	std::fstream file;
	file.open(filename);

	BMS bms;
	while (!file.eof())
	{
		std::string line;
		std::getline(file, line);
		//std::string line = "#00012:000102030405060708090a0b0c0d0e0f0g0h0i0j0k0l0m0n0o0p0q0r0s0t0u0v0w0x0y0z0A0B0C0D0E0F0G0H0I0J0K0L0M0N0O0P0Q0R0S0T0U0V0W0X0Y0Z		; this is test node";
		//std::string line2 = "#PLAYER dfkdufj <dkfjdjfkdkfj>  ; this is test";
		std::smatch match;
		if (std::regex_match(line, match, std::regex("^#([a-zA-Z][a-zA-Z0-9]*)[ \\t]*(.*)(?:[;(?://)].*)?"))) // header
		{
			// name value
			std::string name = match[1];
			std::string value = match[2];
			std::transform(name.begin(), name.end(), name.begin(), ::toupper);

			if (!metadata_only && name.substr(0, 3) == "WAV")
			{
				auto parsed_wav = std::string(match[1]).substr(3, match[1].length() - 3);
				int index = parseValue(parsed_wav.c_str());
				bms.wavs[index] = value;
			}
			if (!metadata_only && name.substr(0, 3) == "BMP")
			{
				int index = parseValue(std::string(match[1]).substr(3, match[1].length() - 3).c_str());
				bms.bmps[index] = value;
			}
			else if (name == "BPM")
			{
				bms.bpm = std::stof(value);
			}
			else if (!metadata_only && name.substr(0, 3) == "BPM")
			{
				int index = parseValue(std::string(match[1]).substr(3, match[1].length() - 3).c_str());
				bms.bpms[index] = std::stof(value);
			}
			else if (name == "PLAYER")
			{
				bms.player = std::stoi(value);
			}
			else if (name == "RANK")
			{
				bms.rank = std::stoi(value);
			}
			else
			{
				bms.metadata[name] = value;
			}
		}
		else if (!metadata_only && std::regex_match(line, match, std::regex("^#([0-9][0-9][0-9])([0-9a-zA-Z][0-9a-zA-Z]):((?:[0-9a-zA-Z][0-9a-zA-Z])*).*")))
		{
			// xxxCH:00-ZZ
			std::string measure = match[1];
			std::string channel = match[2];

			unsigned int imeasure = std::stoi(measure);
			int ichannel = std::stoi(channel);
			if (bms.measures.size() <= imeasure)
				bms.measures.resize(imeasure + 1);
			if (bms.measures[imeasure].nodes.find(ichannel) == bms.measures[imeasure].nodes.end())
				bms.measures[imeasure].nodes[ichannel] = std::list<BMSNode>();

			std::string nodes = match[3];
			size_t len = nodes.length() / 2;
			if (ichannel == CH::BGA || ichannel == CH::Length || ichannel == CH::BPM || ichannel == CH::extendBPM) //for test do not load bmp
				continue;
			auto& channels = bms.measures[imeasure].nodes[ichannel];
			
			for (int i = 0; i < len; i++)
			{
				std::string val = nodes.substr(i * 2, 2);
				int value = parseValue(val.c_str());
				if (value != 0)
				{
					auto node = BMSNode();
					node.value = value;
					node.position = i / (double)len;
					channels.push_back(node);
				}
			}

			/*if (channels.size() < len)
				channels.resize(len);
			auto node = channels.begin();
			/*for (int i = 0; i < len; i++, node++)
			{
				std::string val = nodes.substr(i * 2, 2);
				int value = parseValue(val.c_str());
				if (value != 0)
					node->value = value;
			}*/

			/*
			int i = 0;
			for (auto& it : channels)
			{
				std::string val = nodes.substr(i * 2, 2);
				int value = parseValue(val.c_str());
				i += 1;
				if (value != 0)
					it.value = value;
			}*/
		}
	}
	file.close();

	std::map<int, BMSNode*> lastNodes;
	double measurePosition = 0.0f;
	for (auto& measure : bms.measures)
	{
		measure.position = measurePosition;
		measurePosition += measure.length;
		for (auto& channels : measure.nodes)
		{
			int size = channels.second.size();
			int count = 0;
			for (auto& node : channels.second)	//process position and length
			{
				node.position = measure.length * node.position + measure.position;
				count++;

				if (channels.first >= CH::P1L && channels.first <= (CH::P2L + 7))
				{
					if (bms.lnType == 1)	// ex: 00AA0000FF00
					{
						if (node.value != 0)
						{
							if (lastNodes.find(channels.first) != lastNodes.end() && lastNodes[channels.first] != nullptr)
							{
								lastNodes[channels.first]->length = node.position - lastNodes[channels.first]->position;
								lastNodes[channels.first] = nullptr;
							}
							else
							{
								lastNodes[channels.first] = &node;
							}
						}
					}
					else if (bms.lnType == 2) // ex:: 00AAAAAA00
					{
						if (lastNodes.find(channels.first) != lastNodes.end() && lastNodes[channels.first] != nullptr)
						{
							if (lastNodes[channels.first]->value != node.value)
							{
								lastNodes[channels.first]->length = node.position - lastNodes[channels.first]->position;
								if(node.value != 0)
									lastNodes[channels.first] = &node;
								else
									lastNodes[channels.first] = nullptr;
							}
							else
							{
								node.value = 0; // to remove after
							}
						}
						else if(node.value != 0)
						{
							lastNodes[channels.first] = &node;
						}
					}
				}
				else if (node.value != 0)
				{
					if (bms.ln_objs.find(node.value) != bms.ln_objs.end())
					{
						if (lastNodes.find(channels.first) != lastNodes.end() && lastNodes[channels.first] != nullptr)
							lastNodes[channels.first]->length = node.position - lastNodes[channels.first]->position;
					}
					else
					{
						lastNodes[channels.first] = &node;
					}
				}
			}
			channels.second.remove_if([](auto& i) {return i.value == 0; });
		}
	}
	return bms;
}

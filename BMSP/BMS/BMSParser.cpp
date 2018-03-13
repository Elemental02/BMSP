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

double _4_minute_to_millisecond = 4 * 60 * 1000;

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
	const int PoorBGA = parseValue("06");
	const int BGALayer = parseValue("07");
	const int extendBPM = parseValue("08");
	const int Stop = parseValue("09");
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
		if (std::regex_match(line, match, std::regex("^#([a-zA-Z][a-zA-Z0-9]*)[ \\t]*(.*?)(?:[;(?://)].*)?"))) // header
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
		else if (!metadata_only && std::regex_match(line, match, std::regex("^#([0-9][0-9][0-9])([0-9a-zA-Z][0-9a-zA-Z]):(.*?)(?:[;(?://)].*)?")))
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
			if (ichannel == CH::Length)
			{
				bms.measures[imeasure].length = std::stod(match[3]);
			}
			/*else if (ichannel == CH::BPM)
			{
				//bms.measures[imeasure].bpm = std::stoi(match[3], 0, 16);
			}*/
			else
			{
				auto& channels = bms.measures[imeasure].nodes[ichannel];

				for (int i = 0; i < len; i++)
				{
					std::string val = nodes.substr(i * 2, 2);
					int value = ichannel == CH::BPM ? std::stoi(val, 0, 16) : parseValue(val.c_str());
					double position = i / (double)len;
					if (value != 0)
					{
						auto node = BMSNode();
						node.value = value;
						node.position = position;
						channels.push_back(node);
					}
				}
			}
		}
	}
	file.close();

	std::map<int, BMSNode*> lastNodes;
	double current_bpm = bms.bpm;
	double measurePosition = 0.0f;
	std::chrono::milliseconds measureTime(0);
	for (auto& measure : bms.measures)
	{
		for (auto& channels : measure.nodes)
			channels.second.sort([](auto& lhs, auto& rhs) { return lhs.position < rhs.position; });

		measure.position = measurePosition;
		measurePosition += measure.length;
		measure.time = measureTime;

		if (measure.nodes.find(CH::BPM) == measure.nodes.end())
			measure.nodes[CH::BPM] = std::list<BMSNode>();
		if (measure.nodes.find(CH::extendBPM) == measure.nodes.end())
			measure.nodes[CH::extendBPM] = std::list<BMSNode>();

		auto& bpm_channel = measure.nodes.find(CH::BPM)->second;
		auto& bpm_ext_channel = measure.nodes.find(CH::extendBPM)->second;

		auto& it_bpm = bpm_channel.begin();
		auto& it_bpm_ext = bpm_ext_channel.begin();

		struct bpm_node
		{
			double position;
			double bpm;
			double time;
		};

		std::vector<bpm_node> bpm_nodes;
		bpm_nodes.push_back(bpm_node{ 0, current_bpm, 0 });
		if (bpm_channel.size() >= 2)
		{
			std::cout << "tz"<<std::endl;
		}

		while (it_bpm != bpm_channel.end() || it_bpm_ext != bpm_ext_channel.end())
		{
			auto& back_node = bpm_nodes.back();
			if (it_bpm != bpm_channel.end() && (it_bpm_ext == bpm_ext_channel.end() || it_bpm->position < it_bpm_ext->position))
			{
				double time = back_node.time + (it_bpm->position - back_node.position) * measure.length * _4_minute_to_millisecond / current_bpm;
				it_bpm->a_time = std::chrono::milliseconds(static_cast<long long>(time));
				bpm_nodes.push_back(bpm_node{ it_bpm->position , static_cast<double>(it_bpm->value),time });
				it_bpm++;
			}
			else if (it_bpm_ext != bpm_channel.end() && (it_bpm == bpm_channel.end() || it_bpm->position > it_bpm_ext->position))
			{
				double time = back_node.time + (it_bpm_ext->position - back_node.position) * measure.length * _4_minute_to_millisecond / current_bpm;
				it_bpm_ext->a_time = std::chrono::milliseconds(static_cast<long long>(time));
				//it_bpm_ext->a_time = back_node.time + std::chrono::milliseconds(static_cast<long long>((it_bpm_ext->position - back_node.position) * measure.length * _4_minute_to_millisecond / current_bpm));
				bpm_nodes.push_back(bpm_node{ it_bpm_ext->position , bms.bpms[it_bpm_ext->value],time });
				it_bpm_ext++;
			}
			current_bpm = bpm_nodes.back().bpm;
		}
		
		measure.during_time += std::chrono::milliseconds(static_cast<long long>((1.0 - bpm_nodes.back().position) * measure.length * _4_minute_to_millisecond / current_bpm + bpm_nodes.back().time));

		if (measure.nodes.find(CH::Stop) != measure.nodes.end())
		{
			auto& bpm_node = bpm_nodes.begin();
			std::chrono::milliseconds stopped_time(0);
			for (auto& node : measure.nodes[CH::Stop])
			{
				while ((bpm_node+1) != bpm_nodes.end() && (bpm_node + 1)->position < node.position)
					bpm_node++;
				node.a_time = measure.time + stopped_time + std::chrono::milliseconds(static_cast<long long>((node.position - bpm_node->position) * measure.length * _4_minute_to_millisecond / bpm_node->bpm + bpm_node->time));
				stopped_time += std::chrono::milliseconds(node.value);
			}
			measure.during_time += stopped_time;
		}

		measureTime += measure.during_time;
		for (auto& channels : measure.nodes)
		{
			if (channels.first == CH::BPM || channels.first == CH::extendBPM || channels.first == CH::Stop)
				continue;
			int size = channels.second.size();
			auto& bpm_node = bpm_nodes.begin();
			std::chrono::milliseconds stopped_time(0);
			auto& stop_node = measure.nodes[CH::Stop].begin();
			for (auto& node : channels.second)	//process position and length
			{
				while ((bpm_node + 1) != bpm_nodes.end() && (bpm_node + 1)->position < node.position)
					bpm_node++;
				while ((stop_node) != measure.nodes[CH::Stop].end() && stop_node->position < node.position)
				{
					stopped_time += std::chrono::milliseconds(node.value);
					stop_node++;
				}

				node.a_time = measure.time + stopped_time + std::chrono::milliseconds(static_cast<long long>((node.position - bpm_node->position) * measure.length * _4_minute_to_millisecond / bpm_node->bpm + bpm_node->time));
				node.position = measure.length * node.position + measure.position;

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

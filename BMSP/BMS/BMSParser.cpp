﻿#include "../stdafx.h"
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
std::map<std::string,BMS> BMSParser::bms_list;

BMS BMSParser::Parse(std::string filename, bool metadata_only)
{
	if (bms_list.find(filename) != bms_list.end())
		return bms_list[filename];
	std::fstream file;
	file.open(filename);

	BMS bms;
	while (!file.eof())
	{
		std::string line;
		std::getline(file, line);
		std::smatch match;
		if (std::regex_match(line, match, std::regex("^#([a-zA-Z][a-zA-Z0-9]*)[ \\t]*(.*?)(?:[ \\t]?)(?:;.*)?(?://.*)?"))) // header
		{
			// name value
			std::string name = match[1];
			std::string value = match[2];
			std::transform(name.begin(), name.end(), name.begin(), ::toupper);

			if (/*!metadata_only &&*/ name.substr(0, 3) == "WAV")
			{
				if (metadata_only)
					break;
				auto parsed_wav = std::string(match[1]).substr(3, match[1].length() - 3);
				int index = BMS::parseValue(parsed_wav.c_str());
				bms.wavs[index] = value;
			}
			else if (!metadata_only && name.substr(0, 3) == "BMP")
			{
				int index = BMS::parseValue(std::string(match[1]).substr(3, match[1].length() - 3).c_str());
				bms.bmps[index] = value;
			}
			else if (!metadata_only && name.substr(0, 4) == "STOP")
			{
				int index = BMS::parseValue(std::string(match[1]).substr(4, match[1].length() - 4).c_str());
				bms.stops[index] = std::stoi(value) / 192.0f;
			}
			else if (name == "BPM")
			{
				bms.bpm = std::stof(value);
			}
			else if (!metadata_only && name.substr(0, 3) == "BPM")
			{
				int index = BMS::parseValue(std::string(match[1]).substr(3, match[1].length() - 3).c_str());
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
			else if (name == "LNTYPE")
			{
				bms.lnType = std::stoi(value);
			}
			else
			{
				bms.metadata[name] = value;
			}
		}
		else if (/*!metadata_only &&*/ std::regex_match(line, match, std::regex("^#([0-9][0-9][0-9])([0-9a-zA-Z][0-9a-zA-Z]):(.*?)(?:[ \\t]?)(?:;.*)?(?://.*)?")))
		{
			if (metadata_only)
				break;
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
			if (ichannel == BMS::CH::Length)
			{
				bms.measures[imeasure].length = std::stod(match[3]);
			}
			/*else if (ichannel == BMS::CH::BPM)
			{
				//bms.measures[imeasure].bpm = std::stoi(match[3], 0, 16);
			}*/
			else
			{
				auto& channels = bms.measures[imeasure].nodes[ichannel];

				for (int i = 0; i < len; i++)
				{
					std::string val = nodes.substr(i * 2, 2);
					int value = ichannel == BMS::CH::BPM ? std::stoi(val, 0, 16) : BMS::parseValue(val.c_str());
					double position = i / (double)len;
					if (value != 0 || (ichannel >= BMS::CH::P1L && ichannel <= (BMS::CH::P2L+9)))
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

	if (!metadata_only)
	{
		std::map<int, BMSNode*> lastNodes;
		double current_bpm = bms.bpm;
		double measurePosition = 0.0f;
		std::chrono::milliseconds measureTime(0);

		struct bpm_node
		{
			double position;
			double bpm;
			double time;
			BMSNode* node;
		};
		std::vector<bpm_node> bpm_nodes;
		bpm_nodes.push_back(bpm_node{ 0, current_bpm, 0 });

		for (auto& measure : bms.measures)
		{
			for (auto& channels : measure.nodes)
				channels.second.sort([](auto& lhs, auto& rhs) { return lhs.position < rhs.position; });

			measure.position = measurePosition;
			measurePosition += measure.length;
			measure.time = measureTime;

			if (measure.nodes.find(BMS::CH::BPM) == measure.nodes.end())
				measure.nodes[BMS::CH::BPM] = std::list<BMSNode>();
			if (measure.nodes.find(BMS::CH::extendBPM) == measure.nodes.end())
				measure.nodes[BMS::CH::extendBPM] = std::list<BMSNode>();
			if (measure.nodes.find(BMS::CH::Stop) == measure.nodes.end())
				measure.nodes[BMS::CH::Stop] = std::list<BMSNode>();

			auto& bpm_channel = measure.nodes.find(BMS::CH::BPM)->second;
			auto& bpm_ext_channel = measure.nodes.find(BMS::CH::extendBPM)->second;
			auto& stop_channel = measure.nodes.find(BMS::CH::Stop)->second;

			auto& it_bpm = bpm_channel.begin();
			auto& it_bpm_ext = bpm_ext_channel.begin();
			auto& it_stop = stop_channel.begin();

			while (it_bpm != bpm_channel.end() || it_bpm_ext != bpm_ext_channel.end() || it_stop != stop_channel.end())
			{
				auto& back_node = bpm_nodes.back();
				if (it_bpm != bpm_channel.end() && (it_bpm_ext == bpm_ext_channel.end() || it_bpm->position <= it_bpm_ext->position) && (it_stop == stop_channel.end() || it_bpm->position <= it_stop->position))
				{
					it_bpm->position = measure.length * it_bpm->position + measure.position;
					double time = back_node.time + (it_bpm->position - back_node.position) * BMS::_4_minute_to_millisecond / current_bpm;
					it_bpm->a_time = std::chrono::milliseconds(static_cast<long long>(time));
					bpm_nodes.push_back(bpm_node{ it_bpm->position , static_cast<double>(it_bpm->value),time,&(*it_bpm) });
					it_bpm++;
				}
				else if (it_bpm_ext != bpm_ext_channel.end() && (it_bpm == bpm_channel.end() || it_bpm_ext->position <= it_bpm->position) && (it_stop == stop_channel.end() || it_bpm_ext->position <= it_stop->position))
				{
					it_bpm_ext->position = measure.length * it_bpm_ext->position + measure.position;
					double time = back_node.time + (it_bpm_ext->position - back_node.position) * BMS::_4_minute_to_millisecond / current_bpm;
					it_bpm_ext->a_time = std::chrono::milliseconds(static_cast<long long>(time));
					bpm_nodes.push_back(bpm_node{ it_bpm_ext->position , bms.bpms[it_bpm_ext->value], time,&(*it_bpm_ext) });
					it_bpm_ext++;
				}
				else if (it_stop != stop_channel.end() && (it_bpm == bpm_channel.end() || it_stop->position <= it_bpm->position) && (it_bpm_ext == bpm_ext_channel.end() || it_stop->position <= it_bpm_ext->position))
				{
					it_stop->position = measure.length * it_stop->position + measure.position;
					double time = back_node.time + (it_stop->position - back_node.position) * BMS::_4_minute_to_millisecond / current_bpm;
					it_stop->a_time = std::chrono::milliseconds(static_cast<long long>(time));
					float value = bms.stops[it_stop->value] * (BMS::_4_minute_to_millisecond / current_bpm);
					it_stop->value = static_cast<long long>(value);
					bpm_nodes.push_back(bpm_node{ it_stop->position , current_bpm, time + value,&(*it_stop) });
					it_stop++;
				}
				current_bpm = bpm_nodes.back().bpm;
			}
			measure.during_time = std::chrono::milliseconds(static_cast<long long>((measure.position + measure.length - bpm_nodes.back().position) * BMS::_4_minute_to_millisecond / current_bpm + bpm_nodes.back().time));
			measureTime = measure.during_time;
			//measureTime += measure.during_time;
			for (auto& channels : measure.nodes)
			{
				if (channels.first == BMS::CH::Stop || channels.first == BMS::CH::BPM || channels.first == BMS::CH::extendBPM)
					continue;
				int size = channels.second.size();
				auto& bpm_node = bpm_nodes.begin();
				std::chrono::milliseconds stopped_time(0);
				auto& stop_node = measure.nodes[BMS::CH::Stop].begin();
				for (auto& node : channels.second)	//process position and length
				{
					node.position = measure.length * node.position + measure.position;
					while ((bpm_node + 1) != bpm_nodes.end() && (bpm_node + 1)->position < node.position)
						bpm_node++;
					/*
					while ((stop_node) != measure.nodes[BMS::CH::Stop].end() && stop_node->position < node.position)
					{
						stopped_time += std::chrono::milliseconds(stop_node->value);
						stop_node++;
					}*/
					auto base_position = std::max(bpm_node->position, measure.position);
					auto base_time = std::max(bpm_node->time, static_cast<double>(measure.time.count()));
					node.a_time = stopped_time + std::chrono::milliseconds(static_cast<long long>((node.position - bpm_node->position) * BMS::_4_minute_to_millisecond / bpm_node->bpm + bpm_node->time));

					if (channels.first >= BMS::CH::P1L && channels.first <= (BMS::CH::P2L + 9))
					{
						if (bms.lnType == 1)	// ex: 00AA0000FF00
						{
							if (node.value != 0)
							{
								if (lastNodes.find(channels.first) != lastNodes.end() && lastNodes[channels.first] != nullptr)
								{
									lastNodes[channels.first]->length = node.position - lastNodes[channels.first]->position;
									lastNodes[channels.first] = nullptr;
									node.value = 0;
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
									if (node.value != 0)
										lastNodes[channels.first] = &node;
									else
										lastNodes[channels.first] = nullptr;
								}
								else
								{
									node.value = 0; // to remove after
								}
							}
							else if (node.value != 0)
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
	}
	if (!metadata_only)
		bms_list[filename] = bms;
	return bms;
}

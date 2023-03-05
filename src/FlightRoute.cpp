/*
 * Copyright 2023 Norbert Takacs
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include <iostream>
#include <fstream>
#include <regex>
#include "FlightRoute.h"
#include "Logger.h"

FlightRoute::FlightRoute(std::string _name)
{
	name = _name;
}

std::vector<NavPoint> FlightRoute::get_all_navpoints()
{
	std::vector<NavPoint> all_points;
	std::vector<NavPoint> sid_points = sid.get_nav_points();
	std::vector<NavPoint> star_points = star.get_nav_points();
	std::vector<NavPoint> approach_points = approach.get_nav_points();

	all_points.insert(all_points.end(), sid_points.begin(), sid_points.end());
	all_points.insert(all_points.end(), enroute_points.begin(), enroute_points.end());
	all_points.insert(all_points.end(), star_points.begin(), star_points.end());
	all_points.insert(all_points.end(), approach_points.begin(), approach_points.end());

	return all_points;
}

int FlightRoute::get_start_index_of_phase(RNAVProc::RNAVProcType type)
{
	int index = -1;

	switch (type) {
	case RNAVProc::RNAVProcType::RNAV_SID:
		if (sid.get_nav_points().size() > 0)
			index = 0;
		else
			index = -1;
		break;

	case RNAVProc::RNAVProcType::RNAV_OTHER:
		if (enroute_points.size() > 0)
			index = sid.get_nav_points().size();
		else
			index = -1;
		break;

	case RNAVProc::RNAVProcType::RNAV_STAR:
		if (star.get_nav_points().size() > 0)
			index = sid.get_nav_points().size() + enroute_points.size();
		else
			index = -1;
		break;

	case RNAVProc::RNAVProcType::RNAV_APPROACH:
		if (approach.get_nav_points().size() > 0)
			index = sid.get_nav_points().size() + enroute_points.size() + star.get_nav_points().size();
		else
			index = -1;
		break;
	}

	return index;
}

bool FlightRoute::save_to_file(std::string file_name)
{
	std::ofstream o_str;
	o_str.open(std::filesystem::path(file_name).string(), std::ofstream::out | std::ofstream::trunc);
	if (!o_str.is_open())
	{
		Logger(TLogLevel::logERROR) << "save flight route to file error: can't open file for write: " << file_name << std::endl;
		return false;
	}

	o_str << "dep=" << departure_airport.get_icao_id() << ";" << departure_airport.get_icao_region() << std::endl;
	o_str << "dest=" << destination_airport.get_icao_id() << ";" << destination_airport.get_icao_region() << std::endl;
	o_str << "sid=" << sid.get_name() <<";"<< sid.get_airport_icao_id() << std::endl;
	o_str << "star=" << star.get_name() << ";" << star.get_airport_icao_id() << std::endl;
	o_str << "app=" << approach.get_name() << ";" << approach.get_airport_icao_id() << std::endl;

	int route_point_index = 0;
	for (auto& nav_pt : enroute_points)
	{
		o_str << "route_" << route_point_index << "=" << nav_pt.get_icao_id() << ";" << nav_pt.get_icao_region() << std::endl;
		route_point_index++;
	}

	o_str.close();
	return true;
}

bool FlightRoute::load_from_file(std::string file_name, XPlaneParser& parser)
{
	std::ifstream i_str;
	i_str.open(std::filesystem::path(file_name).string(), std::ofstream::in);
	if (!i_str.is_open())
	{
		Logger(TLogLevel::logERROR) << "load flight route from file error: can't open file for read: " << file_name << std::endl;
		return false;
	}

	//remove all enroute points before load from file
	enroute_points.clear();

	std::string line;
	while (std::getline(i_str, line))
	{
		// remove the leading and trailing whitespace
		std::regex r("^\\s+");
		line = std::regex_replace(line, r, "");
		r = "\\s+$";
		line = std::regex_replace(line, r, "");

		if (line.size() == 0)
			continue;

		std::cmatch m;

		if (std::regex_match(line.c_str(), m, std::regex("dep\\s*=\\s*(.+);(.+)$")))
		{		
			if (!parser.get_airport_by_icao_id(m[1], departure_airport)) {
				Logger(TLogLevel::logERROR) << "error load_from_file: can't load departure airport " << m[1] << std::endl;
				return false;
			}

			continue;
		}

		if (std::regex_match(line.c_str(), m, std::regex("dest\\s*=\\s*(.+);(.+)$")))
		{
			if (!parser.get_airport_by_icao_id(m[1], destination_airport))
			{
				Logger(TLogLevel::logERROR) << "error load_from_file: can't load destination airport " << m[1] << std::endl;
				return false;
			}
			continue;
		}

		if (std::regex_match(line.c_str(), m, std::regex("sid\\s*=\\s*(.+);(.+)$")))
		{
			if (!parser.get_procedure_by_id(m[1], m[2], sid))
			{
				Logger(TLogLevel::logERROR) << "error load_from_file: can't load SID " << m[1] << std::endl;
				return false;
			}
			continue;
		}

		if (std::regex_match(line.c_str(), m, std::regex("star\\s*=\\s*(.+);(.+)$")))
		{
			if (!parser.get_procedure_by_id(m[1], m[2], star))
			{
				Logger(TLogLevel::logERROR) << "error load_from_file: can't load STAR " << m[1] << std::endl;
				return false;
			}
			continue;
		}

		if (std::regex_match(line.c_str(), m, std::regex("app\\s*=\\s*(.+);(.+)$")))
		{
			if (!parser.get_procedure_by_id(m[1], m[2], approach)) 
			{
				Logger(TLogLevel::logERROR) << "error load_from_file: can't load Approach " << m[1] << std::endl;

				return false;
			}
			continue;
		}

		if (std::regex_match(line.c_str(), m, std::regex("route_([0-9]+)\\s*=\\s*(.+);(.+)$")))
		{
			std::list<NavPoint> nav_pts = parser.get_nav_points_by_icao_id(m[3], m[2]);
			if (nav_pts.size() == 0)
			{
				Logger(TLogLevel::logERROR) << "error load_from_file: can't load enroute point " << m[2] << ";" << m[3] << std::endl;
				return false;
			}

			enroute_points.emplace(enroute_points.begin() + std::stoi(m[1]), nav_pts.front());
			continue;
		}

	}
	i_str.close();
	return true;
}

/*
 * Copyright 2023 Norbert Takacs
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
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
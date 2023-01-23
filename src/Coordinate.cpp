/*
 * Copyright 2023 Norbert Takacs
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "pch.h"
#include "Coordinate.h"
#include "Logger.h"
#include "GlobalOptions.h"

Coordinate::Coordinate()
{

}

Coordinate::Coordinate(double _lat, double _lng, double _elevation)
{
	lat = Angle(_lat);
	lng = Angle(_lng);
	elevation = _elevation;
	Logger(TLogLevel::logTRACE) << "Coordinate constructor. lat=" << _lat << " long=" << _lng << " elev=" << _elevation << std::endl;
}

Coordinate::Coordinate(Angle _lat, Angle _lng, double _elevation)
{
	lat = Angle(_lat);
	lng = Angle(_lng);
	elevation = _elevation;
}

Coordinate::Coordinate(const Coordinate& other)
{
	lat = other.lat;
	lng = other.lng;
	elevation = other.elevation;
}

std::string Coordinate::to_string()
{
	std::ostringstream o_str;
	o_str << (lat.get_degree() < 0 ? "S" : "N") << lat.to_string(true) << " " << (lng.get_degree()<0 ? "W" : "E") << lng.to_string(true);

	return o_str.str();
}

Coordinate& Coordinate::operator=(const Coordinate& other)
{
	lat = other.lat;
	lng = other.lng;
	elevation = other.elevation;

	return *this;
}

void Coordinate::get_relative_pos_to(Coordinate& destination, RelativePos& rel_pos)
{
	double Lat1 = lat.convert_to_radian();
	double Lat2 = destination.lat.convert_to_radian();
	double lng1 = lng.convert_to_radian();
	double lng2 = destination.lng.convert_to_radian();

	double earth_radius_km = 6371; //average radius of the Earth in km
	double PI = 3.14159265;

	// calculate orthodrom distance in km units
	rel_pos.dist_ortho = earth_radius_km * (acos(sin(Lat1) * sin(Lat2) + cos(Lat1) * cos(Lat2) * cos(lng2 - lng1)));

	// calculate departure and arrival headings for orthodrom route 
	rel_pos.heading_ortho_departure = (180 / PI) * atan((sin(lng1 - lng2)) / (cos(Lat1) * tan(Lat2) - sin(Lat1) * cos(lng2 - lng1)));
	rel_pos.heading_ortho_arrival = 180 + (180 / PI) * atan((sin(lng1 - lng2)) / (-1*cos(Lat2) * tan(Lat1) + sin(Lat2) * cos(lng2 - lng1)));

	// calculate loxodrom heading	
	double d_lng = 0;
	if (abs(lng1 - lng2) <= PI)
		d_lng = lng1 - lng2;
	else if (lng1 - lng2 < -1 * PI)
		d_lng = 2 * PI + lng1 - lng2;
	else if (lng1 - lng2 > PI)
		d_lng = lng1 - lng2 - 2 * PI;

	rel_pos.heading_loxo = (180 / PI) * atan(d_lng / (log(tan(PI/4+Lat2/2)) - log(tan(PI / 4 + Lat1 / 2))));

	// calculate loxodorm distance
	rel_pos.dist_loxo = earth_radius_km * abs(Lat2 - Lat1) * abs(1 / cos(rel_pos.heading_loxo.convert_to_radian()));
}
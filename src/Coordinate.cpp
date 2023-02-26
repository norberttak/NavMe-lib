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
	lat = Angle(0);
	lng = Angle(0);
	elevation = 0;
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

double Coordinate::calculate_heading_ortho_departure(double lat1, double lng1, double lat2, double lng2)
{
	// calculate departure heading for orthodrom route. see details: http://www.edwilliams.org/avform147.htm#Crs
	double direct_angle = (180 / PI) * std::fmod(atan2(sin(lng2 - lng1) * cos(lat2),
		cos(lat1) * sin(lat2) - sin(lat1) * cos(lat2) * cos(lng1 - lng2)), 2 * PI);

	if (direct_angle < 0)
		direct_angle = direct_angle + 360;

	return direct_angle;
}

double Coordinate::calculate_heading_ortho_arrival(double dep_heading, double lat1, double lng1, double lat2, double lng2)
{
	// calculate arrival heading for orthodrom route
	// use the Clairaut's formula: http://www.edwilliams.org/avform147.htm#Crs
	double direct_angle = 180 - (180 / PI) * asin(sin(PI*dep_heading/180) * cos(lat1) / cos(lat2));

	if (direct_angle < 0)
		direct_angle = direct_angle + 360;
	
	return direct_angle;
}

double Coordinate::calculate_heading_loxo(double lat1, double lng1, double lat2, double lng2)
{
	//see details at http://www.edwilliams.org/avform147.htm#Rhumb
	double dlon_W = std::fmod(lng1 - lng2, 2 * PI);
	double dlon_E = std::fmod(lng2 - lng1, 2 * PI);
	double dphi = log(tan(lat2 / 2 + PI / 4) / tan(lat1 / 2 + PI / 4));
	double q = 0;
	if (abs(lat2 - lat1) < std::sqrt(std::numeric_limits<double>::epsilon())) {
		q = cos(lat1);
	}
	else {
		q = (lat2 - lat1) / dphi;
	}

	double direct_angle = 0;
	double direct_distance = 0;
	if (dlon_W < dlon_E) {// Westerly rhumb line is the shortest
		direct_angle = std::fmod(atan2(-1 * dlon_W, dphi), 2 * PI);
		direct_distance = std::sqrt(std::pow(q,2) * std::pow(dlon_W,2) + std::pow((lat2 - lat1),2));
	}
	else {
		direct_angle = std::fmod(atan2(dlon_E, dphi), 2 * PI);
		direct_distance = std::sqrt(std::pow(q, 2) * std::pow(dlon_E, 2) + std::pow((lat2 - lat1), 2));
	}

	direct_angle = (direct_angle * 180) / PI;
	
	if (direct_angle < 0)
		direct_angle = direct_angle + 360;

	return direct_angle;
}

void Coordinate::get_relative_pos_to(Coordinate& destination, RelativePos& rel_pos)
{
	double lat1 = lat.convert_to_radian();
	double lat2 = destination.lat.convert_to_radian();
	double lng1 = lng.convert_to_radian();
	double lng2 = destination.lng.convert_to_radian();

	// calculate orthodrom (great circle) distance in km units
	rel_pos.dist_ortho = earth_radius_km * (acos(sin(lat1) * sin(lat2) + cos(lat1) * cos(lat2) * cos(lng2 - lng1)));
	// calculate orthodrom headings
	rel_pos.heading_ortho_departure = calculate_heading_ortho_departure(lat1, lng1, lat2, lng2);
	rel_pos.heading_ortho_arrival = calculate_heading_ortho_arrival(rel_pos.heading_ortho_departure.convert_to_double(), lat1, lng1, lat2, lng2);

	// calculate loxodrom (rhumb line) distance in km units
	rel_pos.heading_loxo = calculate_heading_loxo(lat1,lng1,lat2,lng2);
	// calculate loxodorm distance
	rel_pos.dist_loxo = earth_radius_km * abs(lat2 - lat1) * abs(1 / cos(rel_pos.heading_loxo.convert_to_radian()));
}
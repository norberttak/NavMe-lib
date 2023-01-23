/*
 * Copyright 2023 Norbert Takacs
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "pch.h"
#include "NavPoint.h"
#include "Logger.h"

NavPoint::NavPoint(Coordinate _coordinate, std::string _name, std::string _icao_region, Angle _magnetic_variation) :
	coordinate(_coordinate), icao_id(_name), icao_region(_icao_region), magnetic_variation(_magnetic_variation), radio_frequency(0)
{
	Logger(TLogLevel::logTRACE) << "NavPoint constructor: name=" << icao_id << " region=" << icao_region << " coordinate=" << coordinate.to_string() << std::endl;
}

NavPoint::NavPoint() :
 coordinate(), icao_id(""), icao_region(""), magnetic_variation(0), radio_frequency(0)
{

}

NavPoint& NavPoint::operator=(const NavPoint& other)
{
	coordinate = other.coordinate;
	icao_region = other.icao_region;
	icao_id = other.icao_id;
	name = other.name;
	magnetic_variation = other.magnetic_variation;
	radio_type = other.radio_type;
	radio_frequency = other.radio_frequency;
	max_altitude = other.max_altitude;
	min_altitude = other.min_altitude;

	return *this;
}

Coordinate NavPoint::get_coordinate()
{
	return coordinate;
}

Angle NavPoint::get_magnetic_variation()
{
	return magnetic_variation;
}

void NavPoint::set_magnetic_variation(Angle _variation)
{
	magnetic_variation = _variation;
}

void NavPoint::set_coordinate(Coordinate _coord)
{
	coordinate = _coord;
}

void NavPoint::set_icao_region(std::string _region)
{
	icao_region = _region;
}

std::string NavPoint::get_icao_region()
{
	return (icao_region);
}

void NavPoint::set_icao_id(std::string _name)
{
	icao_id = _name;
}

std::string NavPoint::get_icao_id()
{
	return icao_id;
}

void NavPoint::set_name(std::string _name)
{
	name = _name;
}

std::string NavPoint::get_name()
{
	return name;
}

void NavPoint::set_radio_type(RadioNavType _type)
{
	radio_type = _type;
}

NavPoint::RadioNavType NavPoint::get_radio_type()
{
	return radio_type;
}

void NavPoint::set_radio_frequency(int _freq)
{
	radio_frequency = _freq;
}

int NavPoint::get_radio_frequency()
{
	return radio_frequency;
}

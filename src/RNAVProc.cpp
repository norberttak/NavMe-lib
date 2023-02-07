/*
 * Copyright 2023 Norbert Takacs
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "pch.h"
#include "RNAVProc.h"
#include "Logger.h"

RNAVProc::RNAVProc(std::string _name, std::string _icao_region, RNAVProcType _type) :
	name(_name), icao_region(_icao_region), type(_type)
{

}

RNAVProc::RNAVProc():
	name(""), icao_region(""), type(RNAVProcType::RNAV_OTHER)
{

}

RNAVProc::~RNAVProc()
{
	nav_point_ids.clear();
}

void RNAVProc::add_nav_point_id(std::string nav_pnt_id)
{
	nav_point_ids.emplace_back(nav_pnt_id);
}

std::vector<std::string> RNAVProc::get_nav_point_ids()
{
	return nav_point_ids;
}

std::string RNAVProc::get_name()
{
	return name;
}

std::string RNAVProc::get_region()
{
	return icao_region;
}

RNAVProc::RNAVProcType RNAVProc::get_type()
{
	return type;
}

std::string RNAVProc::get_airport_icao_id()
{
	return airport_iaco_id;
}

void RNAVProc::set_airport_iaco_id(std::string _airport_icao_id)
{
	airport_iaco_id = _airport_icao_id;
}

std::string RNAVProc::get_runway_name()
{
	return rwy;
}

void RNAVProc::set_runway_name(std::string _rwy)
{
	rwy = _rwy;
}

RNAVProc& RNAVProc::operator=(const RNAVProc& other)
{
	name = other.name;
	icao_region = other.icao_region;
	airport_iaco_id = other.airport_iaco_id;
	rwy = other.rwy;
	type = other.type;
	nav_point_ids = other.nav_point_ids;

	return *this;
}
/*
 * Copyright 2023 Norbert Takacs
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "pch.h"
#include "Airport.h"
#include "Logger.h"

Runway::Runway(std::string _name, int _course, int _ils_freq, int _length) :
    name(_name), course(_course), ils_freq(_ils_freq), length(_length)
{

}

std::string Runway::get_name()
{
    return name;
}

int Runway::get_course()
{
    return course;
}

int Runway::get_ils_freq()
{
    return ils_freq;
}

int Runway::get_length()
{
    return length;
}

void Runway::set_course(int _course)
{
    course = _course;
}

void Runway::set_ils_freq(int _ils_freq)
{
    ils_freq = _ils_freq;
}

void Runway::set_length(int _length)
{
    length = _length;
}



Airport::Airport(std::string _name, std::string _icao_region, Coordinate _coordinate, double _magnetic_variation) :
    NavPoint(_coordinate, _name, _icao_region, _magnetic_variation)
{
    Logger(TLogLevel::logTRACE) << "Airport created" << icao_id << std::endl;
}

Airport::Airport():
    NavPoint(Coordinate(0,0,0), "","", 0)
{
    
}

Airport& Airport::operator=(const Airport& other)
{
    coordinate = other.coordinate;
    icao_id = other.icao_id;
    icao_region = other.icao_region;
    magnetic_variation = other.magnetic_variation;
    name = other.name;
    
    for (auto rwy : other.runways)
    {
        add_runway(rwy.get_name(),rwy.get_course(),rwy.get_ils_freq(),rwy.get_length());
    }

    return *this;
}

void Airport::add_runway(std::string _name, int _course, int _ils_freq, int _length)
{
    Logger(TLogLevel::logTRACE) << "Airport " << icao_id << " add new runway: " << _name << std::endl;
    runways.emplace_back(_name, _course, _ils_freq, _length);
}

std::list<Runway> Airport::get_runways()
{
    return runways;
}

Airport::~Airport()
{
    runways.clear();
}

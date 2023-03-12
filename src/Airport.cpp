/*
 * Copyright 2023 Norbert Takacs
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "Airport.h"
#include "Logger.h"

Runway::Runway(std::string _name, int _course, int _ils_freq, int _length, int _width) :
    name(_name), course(_course), ils_freq(_ils_freq), length(_length), width(_width)
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

int Runway::get_width()
{
    return width;
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

void Runway::set_width(int _width)
{
    width = _width;
}


Airport::Airport(std::string _name, std::string _icao_region, Coordinate _coordinate, double _magnetic_variation) :
    NavPoint(_coordinate, _name, _icao_region, _magnetic_variation)
{
    transition_alt = 0;
    Logger(TLogLevel::logTRACE) << "Airport created" << icao_id << std::endl;
}

Airport::Airport():
    NavPoint(Coordinate(0,0,0), "","", 0)
{
    transition_alt = 0;
}

Airport& Airport::operator=(const Airport& other)
{
    coordinate = other.coordinate;
    icao_id = other.icao_id;
    icao_region = other.icao_region;
    magnetic_variation = other.magnetic_variation;
    name = other.name;
    iata_id = other.iata_id;
    city = other.city;
    country = other.country;
    state = other.state;
    transition_alt = other.transition_alt;

    for (auto rwy : other.runways)
    {
        add_runway(rwy.get_name(),rwy.get_course(),rwy.get_ils_freq(),rwy.get_length(), rwy.get_width());
    }

    return *this;
}

void Airport::add_runway(std::string _name, int _course, int _ils_freq, int _length, int _width)
{
    Logger(TLogLevel::logTRACE) << "Airport " << icao_id << " add new runway: " << _name << std::endl;
    runways.emplace_back(_name, _course, _ils_freq, _length, _width);
}

std::list<Runway> Airport::get_runways()
{
    return runways;
}

Runway* Airport::get_runway_by_name(std::string rwy_name)
{
    for (auto& rwy : runways)
    {
        if (rwy.get_name() == rwy_name)
            return &rwy;
    }
    return NULL;
}

std::string Airport::get_iata_id()
{
    return iata_id;
}

void Airport::set_iata_id(std::string _iata_id)
{
    iata_id = _iata_id;
}

std::string Airport::get_city()
{
    return city;
}

void Airport::set_city(std::string _city)
{
    city = _city;
}

std::string Airport::get_country()
{
    return country;
}

void Airport::set_country(std::string _country)
{
    country = _country;
}

std::string Airport::get_state()
{
    return state;
}

void Airport::set_state(std::string _state)
{
    state = _state;
}

int Airport::get_transition_alt()
{
    return transition_alt;
}

void Airport::set_transition_alt(int _transition_alt)
{
    transition_alt = _transition_alt;
}

Airport::~Airport()
{
    runways.clear();
}

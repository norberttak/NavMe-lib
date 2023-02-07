/*
 * Copyright 2023 Norbert Takacs
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "pch.h"
#include <sstream>
#include <iomanip>
#include <string>
#include <cmath>
#include "Angle.h"
#include "GlobalOptions.h"

double Angle::convert_to_double()
{
	double res = degree + (double)minute / 60 + (double)second / 3600;
	return sign_negative ? (- 1 * res) : res;
}

double Angle::convert_to_radian()
{
	double angle = convert_to_double();
	return (angle * 3.14159) / 180;
}

void Angle::convert_from_double(double angle)
{
	if (angle < 0)
	{
		degree = (int)floor(-1*angle);
		sign_negative = true;
	}
	else 
	{
		degree = (int)floor(angle);
		sign_negative = false;
	}
	
	minute = (int)((abs(angle) - degree) * 60);
	second = (int)((abs(angle) - degree - ((double)minute / 60)) * 3600);
}

Angle::Angle()
{
	degree = 0;
	minute = 0;
	second = 0;
	sign_negative = false;
}

Angle::Angle(int _degree, int _minute, int _second)
{
	if (_degree < 0) {
		sign_negative = true;
		degree = -1 * _degree;
	}
	else {
		sign_negative = false;
		degree = _degree;
	}

	minute = _minute;
	second = _second;
}

Angle::Angle(double angle)
{
	convert_from_double(angle);
}

Angle::Angle(int _degree, double minute_dec)
{
	if (_degree < 0) {
		sign_negative = true;
		degree = -1 * _degree;
	}
	else {
		sign_negative = false;
		degree = _degree;
	}

	minute = (int)floor(minute_dec);
	second = (int)((minute_dec - minute) * 60);
}

Angle& Angle::operator=(const Angle& other)
{
	degree = other.degree;
	minute = other.minute;
	second = other.second;
	sign_negative = other.sign_negative;

	return *this;
}

Angle& Angle::operator=(const double& d_val)
{
	convert_from_double(d_val);
	return *this;
}

bool Angle::operator==(const Angle& other)
{
	if (degree == other.degree && minute == other.minute && second == other.second && sign_negative == other.sign_negative)
		return true;
	else
		return false;
}

bool Angle::operator!=(const Angle& other)
{
	return !(*this == other);
}

Angle Angle::operator+(Angle& other)
{
	return Angle(this->convert_to_double() + other.convert_to_double());
}

Angle Angle::operator-(Angle& other)
{
	return Angle(this->convert_to_double() - other.convert_to_double());
}

int Angle::get_degree()
{
	return sign_negative ?  (- 1 * degree) : degree;
}

int Angle::get_minute()
{
	return minute;
}

int Angle::get_second()
{
	return second;
}

std::string Angle::to_string(bool use_abs)
{
	//std::setw(4)
	//std::setfill('0')
	std::ostringstream o_str;

	std::string angle_format;
	GlobalOptions::get_instance()->get_option(OPTION_KEY_ANGLE_FORMAT, angle_format);

	if (angle_format == "ANGLE_DEG_DECMIN")
	{
		o_str << ((use_abs==false && sign_negative)?"-":"") << std::setprecision(4) << (use_abs ? abs(degree) : degree) << 'o' << (double)(minute + ((double)second / 60)) << "'";
	}
	else if (angle_format == "ANGLE_DEG_MIN_SEC")
	{
		o_str << ((use_abs == false && sign_negative) ? "-" : "") << std::setprecision(4) << (use_abs ? abs(degree) : degree) << 'o' << minute << "'" << second << "\"";
	}
	else if (angle_format == "ANGLE_DOUBLE")
	{
		o_str << std::setprecision(4) << (use_abs ? abs(convert_to_double()) : convert_to_double());
	}
	
	return o_str.str();
}
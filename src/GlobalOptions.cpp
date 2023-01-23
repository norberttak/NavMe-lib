/*
 * Copyright 2023 Norbert Takacs
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "pch.h"
#include <sstream>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <regex>
#include "GlobalOptions.h"
#include "Logger.h"

GlobalOptions* GlobalOptions::instance = nullptr;

GlobalOptions* GlobalOptions::get_instance() {
	if (!instance)
		instance = new GlobalOptions();
	return instance;
}

bool GlobalOptions::set_option(std::string option_name, std::string option_value)
{
	std::transform(option_name.begin(), option_name.end(), option_name.begin(), ::toupper);
	key_values[option_name] = option_value;
	return true;
}

bool GlobalOptions::get_option(std::string option_name, std::string& option_value)
{
	std::transform(option_name.begin(), option_name.end(), option_name.begin(), ::toupper);
	if (key_values.count(option_name) == 0)
	{
		//Logger(TLogLevel::logWARNING) << "GlobalOptions get_option(" << option_name << ") unknown key" << std::endl;
		return false;
	}

	option_value = key_values[option_name];
	//Logger(TLogLevel::logTRACE) << "GlobalOptions get_option(" << option_name << ") value:" << option_value << std::endl;
	return true;
}

std::list<std::string> GlobalOptions::get_all_option_name()
{
	std::list<std::string> key_names;
	for (auto& option : key_values)
	{
		key_names.emplace_back(option.first);
	}

	return key_names;
}

std::filesystem::path GlobalOptions::normalize_file_path(std::string file_name)
{
	std::filesystem::path config_path(file_name);
	if (std::filesystem::exists(config_path))
	{
		return config_path;
	}
	else
	{
		config_path = std::filesystem::current_path();
		config_path /= file_name;
		return config_path;
	}
}

bool GlobalOptions::save_options_to_file(std::string options_file_name)
{	
	std::ofstream o_str;
	o_str.open(normalize_file_path(options_file_name).string(), std::ofstream::out | std::ofstream::trunc);
	if (!o_str.is_open())
	{
		Logger(TLogLevel::logERROR) << "save_options_to_file: can't open file for write: " << options_file_name << std::endl;
		return false;
	}

	for (auto &option : key_values)
	{
		o_str << option.first << "=" << option.second << std::endl;
		Logger(TLogLevel::logTRACE) << "save_options_to_file[" << option.first << "]=" << option.second << std::endl;
	}
	
	o_str.close();
	return true;
}

bool GlobalOptions::load_options_from_file(std::string options_file_name)
{	
	std::ifstream i_str;
	i_str.open(normalize_file_path(options_file_name).string(), std::ofstream::in);
	if (!i_str.is_open())
	{
		Logger(TLogLevel::logERROR) << "load_options_from_file: can't open file for read: " << options_file_name << std::endl;
		return false;
	}
	
	std::string line;
	while (std::getline(i_str, line)) 
	{
		// comments are marked by semicolon (;)
		size_t pos = line.find(';', 0);
		if (pos != std::string::npos)
			line.erase(line.find(';', 0), line.length() - pos);

		// remove the leading and trailing whitespace
		std::regex r("^\\s+");
		line = std::regex_replace(line, r, "");
		r = "\\s+$";
		line = std::regex_replace(line, r, "");

		if (line.size() == 0)
			continue;
		std::cmatch m;

		if (std::regex_match(line.c_str(), m, std::regex("(\\S+)\\s*=\\s*(.+)$")))
		{
			key_values[m[1]] = m[2];
			Logger(TLogLevel::logTRACE) << "load_options_from_file: " << m[1] << "=" << m[2] << std::endl;
		}
	}

	i_str.close();
	return true;
}
/*
 * Copyright 2023 Norbert Takacs
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "XPlaneParser.h"
#include "../NavMeLib.h"
#include "../Logger.h"

std::filesystem::path XPlaneParser::absolute_path(std::string root_folder, std::string nav_folder, std::string file_name)
{
	std::filesystem::path init_path = std::filesystem::path(root_folder);
	//init_path = init_path.remove_filename();
	init_path /= nav_folder;
	init_path /= file_name;

	return init_path;
}

void XPlaneParser::trim_line(std::string& line)
{
	// remove the leading and trailing whitespace
	std::regex r("^\\s+");
	line = std::regex_replace(line, r, "");
	r = "\\s+$";
	line = std::regex_replace(line, r, "");
}

bool XPlaneParser::parse_earth_fix_dat_file()
{
	std::filesystem::path file_absolute_path = absolute_path(xplane_root_folder, "Custom Data", "earth_fix.dat");
	std::ifstream i_str;
	i_str.open(file_absolute_path);
	if (!i_str.is_open())
	{
		Logger(TLogLevel::logERROR) << "parse_earth_fix_dat_file: can't open file for read: earth_fix.dat" << std::endl;
		Logger(TLogLevel::logINFO) << "full path: " << file_absolute_path.string() << std::endl;
		return false;
	}

	std::string line;
	int line_count = 0;
	while (std::getline(i_str, line))
	{
		if (++line_count <= 3 || line.length() < 50)
			continue;

		//-21.014086111   26.872350000  ABFNV ENRT FB 2115154 ABEAM FRANCISTOWN VOR
		// 47.483388889   18.258777778  GILEP ENRT LH 4478275 GILEP
		//0123456789012234567890123456789012345678901234567890123456789
		//          1          2         3         4         5
		_nav_points.emplace_back(Coordinate(Angle(std::stod(line.substr(0, 13))), Angle(std::stod(line.substr(14, 13))), 0), line.substr(30, 5), line.substr(41, 2), 0);
	}

	i_str.close();
	return true;
}

bool XPlaneParser::parse_earth_nav_dat_file()
{
	std::filesystem::path file_absolute_path = absolute_path(xplane_root_folder, "Custom Data", "earth_nav.dat");
	std::ifstream i_str;
	i_str.open(file_absolute_path);
	if (!i_str.is_open())
	{
		Logger(TLogLevel::logERROR) << "parse_earth_nav_dat_file: can't open file for read: earth_nav.dat" << std::endl;
		Logger(TLogLevel::logINFO) << "full path: " << file_absolute_path.string() << std::endl;
		return false;
	}

	std::string line;
	int line_count = 0;

	while (std::getline(i_str, line))
	{
		if (++line_count <= 3)
			continue;

		if (line.length() <= 80)
			continue;

		// 3  47.152222222   18.742222222      430    11710   130      5.000  PTB ENRT LH PUSZTASZABOLCS VOR/DME
		//12  47.152222222   18.742222222      430    11710   130      0.000  PTB ENRT LH PUSZTASZABOLCS VOR/DME
		// 4  47.420805556   19.297333333      499    10915    18  45852.474  BPL LHBP LH 13L ILS-cat-II  
		//0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890
		//          1         2         3         4         5         6         7         8         9
		int type = std::stoi(line.substr(0, 2)); // type
		double lat = std::stod(line.substr(4, 13));// lat
		double lng = std::stod(line.substr(19, 13));// lng
		int alt = std::stoi(line.substr(35, 5)); // alt
		int freq = std::stoi(line.substr(44, 5)); //freq
		double course_combined = std::stod(line.substr(56, 10)); // ILS course. 360 x magnetic course + true course
		std::string id = line.substr(67, 4); //id
		id = id.substr(id.find_first_not_of(' ', 0));
		std::string ils_airport_icao = line.substr(72, 4); // only for ILS
		std::string ils_rwy_name = line.substr(80, 3); // only for ILS
		std::string ils_name = line.substr(84); // only for ILS
		std::string region = line.substr(77, 2); //region
		std::string name = line.substr(80); //name

		int true_course = 0;
		int magnetic_course = 0;
		Airport* airport_ptr = NULL;
		bool rwy_found = false;

		switch (type)
		{
		case 2:
			_nav_points.emplace_back(Coordinate(Angle(lat), Angle(lng), alt), id, region, 0);
			_nav_points.back().set_radio_type(NavPoint::NDB);
			_nav_points.back().set_radio_frequency(freq);
			_nav_points.back().set_name(name);
			break;
		case 12:
			// DME collocated with DME. the VOR should be the previously parsed item
			if (_nav_points.back().get_icao_id() == id)
				_nav_points.back().set_radio_type(NavPoint::VOR_DME);
			else
				Logger(TLogLevel::logERROR) << "XplaneParser: VOR-DME detected but can't find the VOR entity: " << id << std::endl;
			break;
		case 13:
			_nav_points.emplace_back(Coordinate(Angle(lat), Angle(lng), alt), id, region, 0);
			_nav_points.back().set_radio_type(NavPoint::DME);
			_nav_points.back().set_radio_frequency(freq);
			_nav_points.back().set_name(name);
			break;
		case 3:
			true_course = (int)fmod(course_combined, 360);
			magnetic_course = (int)((course_combined - true_course) / 360);

			_nav_points.emplace_back(Coordinate(Angle(lat), Angle(lng), alt), id, region, 0);
			_nav_points.back().set_radio_type(NavPoint::VOR);
			_nav_points.back().set_radio_frequency(freq);
			_nav_points.back().set_name(name);
			_nav_points.back().set_magnetic_variation(true_course - magnetic_course);
			break;
		case 4:
			true_course = (int)fmod(course_combined, 360);
			magnetic_course = (int)((course_combined - true_course) / 360);

			airport_ptr = get_airport_ptr(ils_airport_icao);
			if (!airport_ptr)
			{
				_airports.emplace_back(ils_airport_icao, ils_airport_icao.substr(0, 2), Coordinate(lat, lng, alt), true_course - magnetic_course);
				airport_ptr = &_airports.back();
			}

			airport_ptr->set_magnetic_variation(true_course - magnetic_course);

			for (auto& rwy : airport_ptr->get_runways())
			{
				if (rwy.get_name() == ils_rwy_name)
				{
					rwy.set_course(magnetic_course);
					rwy.set_ils_freq(freq);
					rwy_found = true;
				}
			}
			if (!rwy_found)
				airport_ptr->add_runway(ils_rwy_name, magnetic_course, freq, 0);
			break;

		default:
			break;
		}
	}

	i_str.close();
	return true;
}

void XPlaneParser::parse_rwy_line(std::cmatch& m, Airport* apt_ptr)
{
	//N47264352
	double lat = 0;
	std::string lat_str = m[4];
	trim_line(lat_str);
	lat_str.insert(3, 1, '.');
	if (lat_str[0] == 'N')
		lat = std::stod(lat_str.substr(1, lat_str.length()));
	else
		lat = -1 * std::stod(lat_str.substr(1, lat_str.length()));

	//E019152718
	double lng = 0;
	std::string lng_str = m[5];
	trim_line(lng_str);
	lng_str.insert(4, 1, '.');
	if (lng_str[0] == 'E')
		lng = std::stod(lng_str.substr(1, lng_str.length()));
	else
		lng = -1 * std::stod(lng_str.substr(1, lng_str.length()));

	apt_ptr->set_coordinate(Coordinate(lat, lng, std::stoi(m[2])));

	//name,course,ils,length
	apt_ptr->add_runway(m[1], 0, 0, 0);
}

//APPCH:010,A,I31R,ATICO,ATICO,LH,P,C,E  A, ,   ,IF, , , , , ,      ,    ,    ,    ,    ,+,04000,     ,     ,-,230,    ,   , , , , , ,0,N,S;
//APPCH:020, A, I31R, ATICO, BP865, LH, P, C, EE B, , , TF, , BPR, LH, P, I, , , , , , +, 03000, , , -, 230, , , , , , , , 0, N, S;
void XPlaneParser::parse_approach_proc_line(std::cmatch& m, std::string airport_iaco_id)
{
	if (m[3] != "A")
		return;

	RNAVProc::RNAVProcType proc_type = RNAVProc::RNAVProcType::RNAV_APPROACH;

	if (m[2] == "010")
	{
		std::string app_name_with_transition = std::string(m[4]) + "-" + std::string(m[5]);

		_rnav_procs.emplace_back(app_name_with_transition, m[7], proc_type);
		_rnav_procs.back().set_airport_iaco_id(airport_iaco_id);
		_rnav_procs.back().add_nav_point_id(m[6]);
	}
	else
	{
		_rnav_procs.back().add_nav_point_id(m[6]);
	}
}

void XPlaneParser::parse_proc_line(std::cmatch& m, std::string airport_icao_id)
{
	RNAVProc::RNAVProcType proc_type;
	if (m[1] == "SID")
		proc_type = RNAVProc::RNAVProcType::RNAV_SID;
	else if (m[1] == "STAR")
		proc_type = RNAVProc::RNAVProcType::RNAV_STAR;
	else if (m[1] == "APPCH") {
		return parse_approach_proc_line(m, airport_icao_id);
	}
	else
		proc_type = RNAVProc::RNAVProcType::RNAV_OTHER;

	bool rnav_proc_already_exists = false;
	for (auto it = _rnav_procs.begin(); it != _rnav_procs.end(); it++)
	{
		if (it->get_name() == m[4] && it->get_airport_icao_id() == airport_icao_id)
		{
			it->add_nav_point_id(m[6]);
			rnav_proc_already_exists = true;
		}
	}

	if (!rnav_proc_already_exists)
	{
		_rnav_procs.emplace_back(m[4], airport_icao_id.substr(0, 2), proc_type);
		_rnav_procs.back().add_nav_point_id(m[6]);
		_rnav_procs.back().set_runway_name(m[5]);
		_rnav_procs.back().set_airport_iaco_id(airport_icao_id);
	}

}

bool XPlaneParser::parse_airport_file(std::string airport_icao_code)
{
	std::string file_name = airport_icao_code + ".dat";
	std::filesystem::path file_path = std::filesystem::path(xplane_root_folder) / "Custom Data" / "CIFP" / file_name;

	Logger(TLogLevel::logTRACE) << "Parse airport file: " << file_path << std::endl;

	std::ifstream i_str;
	i_str.open(file_path);
	if (!i_str.is_open())
	{
		Logger(TLogLevel::logERROR) << "parse_airport_file: can't open file for read: " << file_path << std::endl;
		return false;
	}

	Airport* apt_ptr = get_airport_ptr(airport_icao_code);
	if (apt_ptr == NULL)
	{
		_airports.emplace_back();
		apt_ptr = &_airports.back();
	}

	apt_ptr->set_icao_id(airport_icao_code);
	apt_ptr->set_icao_region(airport_icao_code.substr(0, 2));

	std::string line;
	auto re_apt_rwy_line = std::regex(APT_RWY_LINE);
	auto re_apt_proc_line = std::regex(APT_PROC_LINE);
	while (std::getline(i_str, line))
	{
		trim_line(line);

		std::cmatch m;
		if (std::regex_match(line.c_str(), m, re_apt_rwy_line))
		{
			parse_rwy_line(m, apt_ptr);
		}
		else if (std::regex_match(line.c_str(), m, re_apt_proc_line))
		{
			parse_proc_line(m, airport_icao_code);
		}
	}

	return true;
}

std::list<RNAVProc> XPlaneParser::get_rnav_procs_by_airport_icao_id(std::string icao_id)
{
	std::list<RNAVProc> rnav_procs;

	bool airport_parsed = false;
	for (auto& ap : _airports)
		if (ap.get_icao_id() == icao_id)
			airport_parsed = true;

	if (!airport_parsed)
		if (!parse_airport_file(icao_id))
			return rnav_procs;

	for (auto& proc : _rnav_procs) {
		if (proc.get_airport_icao_id() == icao_id)
			rnav_procs.emplace_back(proc);
	}

	return rnav_procs;
}

std::list<std::string> XPlaneParser::get_list_of_airport_iaco_codes()
{
	std::list<std::string> icao_codes;
	for (auto& dir_entry : std::filesystem::directory_iterator{ std::filesystem::path(xplane_root_folder) / "CIFP" })
		icao_codes.emplace_back(dir_entry.path().filename().replace_extension().string());

	return icao_codes;
}

XPlaneParser::XPlaneParser(std::string _xplane_root_folder)
{
	xplane_root_folder = _xplane_root_folder;
}

std::list<NavPoint>& XPlaneParser::get_nav_points()
{
	return _nav_points;
}

std::list<NavPoint> XPlaneParser::get_nav_points_by_icao_id(std::string icao_id)
{
	return get_nav_points_by_icao_id("all", icao_id);
}

std::list<NavPoint> XPlaneParser::get_nav_points_by_icao_id(std::string region, std::string icao_id)
{
	std::list<NavPoint> return_list;

	for (auto& nav_point : _nav_points)
	{
		if (nav_point.get_icao_id() == icao_id && (region == "all" || nav_point.get_icao_region() == region))
			return_list.emplace_back(nav_point);
	}
	return return_list;
}

bool XPlaneParser::get_airport_by_icao_id(std::string icao_id, Airport& _airport)
{
	for (auto& ap : _airports)
	{
		if (ap.get_icao_id() == icao_id)
		{
			_airport = ap;
			return true;
		}
	}

	if (!parse_airport_file(icao_id))
	{
		Logger(TLogLevel::logERROR) << "Can't open airport file for " << icao_id << std::endl;
		return false;
	}

	for (auto& ap : _airports)
	{
		if (ap.get_icao_id() == icao_id)
		{
			_airport = ap;
			return true;
		}
	}

	Logger(TLogLevel::logERROR) << "Can't find airport " << icao_id << std::endl;
	return false;
}

Airport* XPlaneParser::get_airport_ptr(std::string airport_icao_code)
{
	for (auto& ap : _airports)
	{
		if (ap.get_icao_id() == airport_icao_code)
		{
			return &ap;
		}
	}
	return NULL;
}

bool XPlaneParser::get_procedure_by_id(std::string proc_name, std::string airport_icao, RNAVProc& proc)
{
	parse_airport_file(airport_icao);

	for (auto it = _rnav_procs.begin(); it != _rnav_procs.end(); it++)
	{
		if (it->get_name() == proc_name && it->get_airport_icao_id() == airport_icao) {
			proc = *it;
			return true;
		}
	}
	return false;
}
#include "pch.h"
#include <iostream>
#include <filesystem>
#include "CppUnitTest.h"
#include "NavMeLib.h"
#include "XPlane-navdata-parser\XPlaneParser.h"
#include "Logger.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace test
{
	TEST_CLASS(TestXPlaneParser)
	{
	private:
		std::filesystem::path nav_data_path;
	public:
		TEST_METHOD_INITIALIZE(TestXPlaneParserInit)
		{
			nav_data_path = std::filesystem::current_path();
			nav_data_path /= "../../test/test-data";
		}
		TEST_METHOD(TestGetNavPointByName)
		{
			XPlaneParser parser(nav_data_path.string());
			parser.parse_earth_fix_dat_file();
			parser.parse_earth_nav_dat_file();
			NavPoint nav_point;
			
			std::list<NavPoint> result = parser.get_nav_points_by_icao_id("ABGOL");
			Assert::AreEqual(1, (int)result.size());
			NavPoint top_element(result.front());

			//35.917222222   10.501666667  ABGOL ENRT DT 4478290 ABGOL
			Assert::AreEqual(35.917222222, top_element.get_coordinate().lat.convert_to_double(), 0.1);
			Assert::AreEqual(10.501666667, top_element.get_coordinate().lng.convert_to_double(), 0.1);
			Assert::AreEqual("ABGOL", top_element.get_icao_id().c_str());
			Assert::AreEqual("DT", top_element.get_icao_region().c_str());
		}

		TEST_METHOD(TestGetVORbyName)
		{
			XPlaneParser parser(nav_data_path.string());
			parser.parse_earth_fix_dat_file();
			parser.parse_earth_nav_dat_file();
			NavPoint nav_point;

			std::list<NavPoint> result = parser.get_nav_points_by_icao_id("PTB");
			Assert::AreEqual(1, (int)result.size());
			NavPoint top_element(result.front());

			// 3  47.152222222   18.742222222      430    11710   130      5.000  PTB ENRT LH PUSZTASZABOLCS VOR/DME
			//12  47.152222222   18.742222222      430    11710   130      0.000  PTB ENRT LH PUSZTASZABOLCS VOR / DME

			Assert::AreEqual(47.152222222, top_element.get_coordinate().lat.convert_to_double(), 0.1);
			Assert::AreEqual(18.742222222, top_element.get_coordinate().lng.convert_to_double(), 0.1);
			Assert::AreEqual("PTB", top_element.get_icao_id().c_str());
			Assert::AreEqual("LH", top_element.get_icao_region().c_str());
			Assert::AreEqual(11710, top_element.get_radio_frequency());
			Assert::AreEqual((int)NavPoint::VOR_DME, (int)top_element.get_radio_type());
		}

		TEST_METHOD(TestParseAirportFile_LOWI)
		{
			XPlaneParser parser(nav_data_path.string());
			parser.parse_earth_fix_dat_file();
			parser.parse_earth_nav_dat_file();
			
			Airport airport;
			parser.get_airport_by_icao_id("LOWI", airport);
			Assert::AreEqual(2, (int)airport.get_runways().size());
		}

		TEST_METHOD(TestParseAirportFile_KSEA)
		{
			XPlaneParser parser(nav_data_path.string());
			parser.parse_earth_fix_dat_file();
			parser.parse_earth_nav_dat_file();

			Airport airport;
			parser.get_airport_by_icao_id("KSEA", airport);
			Assert::AreEqual(6, (int)airport.get_runways().size());

			GlobalOptions::get_instance()->set_option(OPTION_KEY_ANGLE_FORMAT, "ANGLE_DEG_MIN_SEC");
			Assert::AreEqual("N47ø15'18\" W122ø10'58\"", airport.get_coordinate().to_string().c_str());
		}

		TEST_METHOD(TestGetRNAVProcbyName)
		{
			XPlaneParser parser(nav_data_path.string());
			parser.parse_earth_fix_dat_file();
			parser.parse_earth_nav_dat_file();
			
			RNAVProc proc;
			bool result = parser.get_procedure_by_id("BADO2B", "LHBP", proc);
			Assert::IsTrue(result);

			Assert::AreEqual("LHBP", proc.get_airport_icao_id().c_str());
			Assert::AreEqual("BADO2B", proc.get_name().c_str());
			Assert::AreEqual("LH", proc.get_region().c_str());
			Assert::AreEqual("RW13L", proc.get_runway_name().c_str());
			Assert::IsTrue(RNAVProc::RNAVProcType::RNAV_SID == proc.get_type());
			std::vector<NavPoint> nav_ids = proc.get_nav_points();
			Assert::AreEqual(6, (int)nav_ids.size());

			Assert::AreEqual("DE13L", nav_ids[0].get_icao_id().c_str());
			Assert::AreEqual("BP701", nav_ids[1].get_icao_id().c_str());
			Assert::AreEqual("BP702", nav_ids[2].get_icao_id().c_str());
			Assert::AreEqual("BP703", nav_ids[3].get_icao_id().c_str());
			Assert::AreEqual("BP704", nav_ids[4].get_icao_id().c_str());
			Assert::AreEqual("BADOV", nav_ids[5].get_icao_id().c_str());
		}

		TEST_METHOD(TestProcudureApproach)
		{
			XPlaneParser parser(nav_data_path.string());
			parser.parse_earth_fix_dat_file();
			parser.parse_earth_nav_dat_file();

			RNAVProc proc;
			bool result = parser.get_procedure_by_id("I31R-ATICO", "LHBP", proc);
			Assert::IsTrue(result);

			Assert::AreEqual("LHBP", proc.get_airport_icao_id().c_str());
			Assert::AreEqual("LH", proc.get_region().c_str());
			Assert::IsTrue(RNAVProc::RNAVProcType::RNAV_APPROACH == proc.get_type());
			std::vector<NavPoint> nav_ids = proc.get_nav_points();
			Assert::AreEqual(2, (int)nav_ids.size());

			Assert::AreEqual("ATICO", nav_ids[0].get_icao_id().c_str());
			Assert::AreEqual("BP865", nav_ids[1].get_icao_id().c_str());
		}

		TEST_METHOD(TestAirportIlsData)
		{
			XPlaneParser parser(nav_data_path.string());
			parser.parse_earth_fix_dat_file();
			parser.parse_earth_nav_dat_file();

			Airport apt;
			parser.get_airport_by_icao_id("LHBP", apt);

			Assert::AreEqual("LHBP", apt.get_icao_id().c_str());
			std::list<Runway> rwys = apt.get_runways();
			Assert::AreEqual(4, (int)rwys.size());

		}

		TEST_METHOD_CLEANUP(TestXPlaneParserCleanup)
		{

		}

	};
}

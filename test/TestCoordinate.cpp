#include "pch.h"
#include "CppUnitTest.h"
#include "NavMeLib.h"
#include "Logger.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace test
{
	TEST_CLASS(TestCoordinate)
	{
	private:
		//
	public:
		TEST_METHOD_INITIALIZE(TestCoordinateInit)
		{

		}

		TEST_METHOD(TestComputeRouteOrthodrom)
		{
			/* Calculate Orthodrom (greate circle) route between
			   Seattle (KSEA) és London (EGLL)
			   KSEA lat1 = 47o26'56"N long1 = 122o18'34"W
			   EGLL lat2 = 51o28'39"N long2 = 000o27'41"W
			*/

			Coordinate coord_ksea(Angle(47, 26, 56), Angle(122, 18, 34), 0);
			Coordinate coord_egll(Angle(51, 28, 39), Angle(0, 27, 41), 0);

			RelativePos relative_pos;
			coord_ksea.get_relative_pos_to(coord_egll, relative_pos);

			// great circle distance between KSEA - EGLL is 7701 km
			Assert::AreEqual(7701, int(relative_pos.dist_ortho));
			
			// departure heading shall be 34o27'6"
			Assert::AreEqual(34, relative_pos.heading_ortho_departure.get_degree());
			Assert::AreEqual(27, relative_pos.heading_ortho_departure.get_minute());
			Assert::AreEqual(6, relative_pos.heading_ortho_departure.get_second());

			// arrival heading shall be 142o6'12"
			Assert::AreEqual(142, relative_pos.heading_ortho_arrival.get_degree());
			Assert::AreEqual(6, relative_pos.heading_ortho_arrival.get_minute());
			Assert::AreEqual(12, relative_pos.heading_ortho_arrival.get_second());
		}

		TEST_METHOD(TestComputeRouteLoxodrom)
		{
			/* Calculate Loxodrom route between
			   Seattle (KSEA) és London (EGLL)
			   KSEA lat1 = 47o26'56"N long1 = 122o18'34"W
			   EGLL lat2 = 51o28'39"N long2 = 000o27'41"W
			*/

			Coordinate coord_ksea(Angle(47, 26, 56), Angle(122, 18, 34), 0);
			Coordinate coord_egll(Angle(51, 28, 39), Angle(0, 27, 41), 0);

			RelativePos relative_pos;
			coord_ksea.get_relative_pos_to(coord_egll, relative_pos);

			// small circle distance between KSEA - EGLL is 8810 km
			Assert::AreEqual(8810, int(relative_pos.dist_loxo));

			// the constant loxodrom heading shall be 87o5'8"
			Assert::AreEqual(87, relative_pos.heading_loxo.get_degree());
			Assert::AreEqual(5, relative_pos.heading_loxo.get_minute());
			Assert::AreEqual(8, relative_pos.heading_loxo.get_second());
		}

		TEST_METHOD(TestCoordinateToStringDegMinSec)
		{
			Coordinate coord_ksea(Angle(47, 26, 56), Angle(122, 18, 34), 0);
			GlobalOptions::get_instance()->set_option(OPTION_KEY_ANGLE_FORMAT, "ANGLE_DEG_MIN_SEC");

			std::string coord_str = coord_ksea.to_string();
			Assert::AreEqual("N47o26\'56\" E122o18\'34\"",coord_str.c_str());
		}

		TEST_METHOD(TestCoordinateToStringDegDecMin)
		{
			Coordinate coord_ksea(Angle(47, 26, 56), Angle(122, 18, 34), 0);
			GlobalOptions::get_instance()->set_option(OPTION_KEY_ANGLE_FORMAT, "ANGLE_DEG_DECMIN");

			std::string coord_str = coord_ksea.to_string();
			Assert::AreEqual("N47o26.93\' E122o18.57\'", coord_str.c_str());
		}

		TEST_METHOD(TestCoordinateToStringDegDouble)
		{
			Coordinate coord_ksea(Angle(47, 26, 56), Angle(122, 18, 34), 0);
			GlobalOptions::get_instance()->set_option(OPTION_KEY_ANGLE_FORMAT, "ANGLE_DOUBLE");

			std::string coord_str = coord_ksea.to_string();
			Assert::AreEqual("N47.45 E122.3", coord_str.c_str());
		}

		TEST_METHOD_CLEANUP(TestAngleCleanup)
		{

		}

	};
}
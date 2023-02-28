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
			   Seattle (KSEA) to London (EGLL)
			   KSEA lat1 = 47o26'56"N long1 = 122o18'34"W
			   EGLL lat2 = 51o28'39"N long2 = 000o27'41"W(=-0.4613889)
			*/

			Coordinate coord_ksea(Angle(47, 26, 56), Angle(-122, 18, 34), 0);
			Coordinate coord_egll(Angle(51, 28, 39), Angle(-0.4613889), 0);

			RelativePos relative_pos;
			coord_ksea.get_relative_pos_to(coord_egll, relative_pos);

			// great circle distance between KSEA - EGLL is 7740 km
			Assert::AreEqual(7701, int(relative_pos.dist_ortho));
			
			// departure heading shall be 34o27'6"
			Assert::AreEqual(34, relative_pos.heading_ortho_departure.get_degree(), L"departure deg");
			Assert::AreEqual(27, relative_pos.heading_ortho_departure.get_minute(), L"departure min");
			Assert::AreEqual(6, relative_pos.heading_ortho_departure.get_second(), L"departure sec");

			// arrival heading shall be 142o6'12"
			Assert::AreEqual(142, relative_pos.heading_ortho_arrival.get_degree(), L"arrival deg");
			Assert::AreEqual(6, relative_pos.heading_ortho_arrival.get_minute(), L"arrival min");
			Assert::AreEqual(12, relative_pos.heading_ortho_arrival.get_second(), L"arrival sec");
		}

		TEST_METHOD(TestComputeRouteLoxodrom_KSEA_EGLL)
		{
			/* Calculate Loxodrom route between
			   Seattle (KSEA) to London (EGLL)
			   KSEA lat1 = 47o26'56"N long1 = 122o18'34"W
			   EGLL lat2 = 51o28'39"N long2 = 000o27'41"W(=-0.4613889)
			*/

			Coordinate coord_ksea(Angle(47, 26, 56), Angle(-122, 18, 34), 0);
			Coordinate coord_egll(Angle(51, 28, 39), Angle(-0.4613889), 0);

			RelativePos relative_pos;
			coord_ksea.get_relative_pos_to(coord_egll, relative_pos);

			// rhumb line distance
			Assert::AreEqual(8810, int(relative_pos.dist_loxo));

			// the constant loxodrom heading shall be 87o5'8"
			Assert::AreEqual(87, relative_pos.heading_loxo.get_degree());
			Assert::AreEqual(5, relative_pos.heading_loxo.get_minute());
			Assert::AreEqual(8, relative_pos.heading_loxo.get_second());
		}

		TEST_METHOD(TestComputeRouteLoxodrom_EGLL_KSEA)
		{
			/* Calculate Loxodrom route between
			   London (EGLL) to Seattle (KSEA)
			   KSEA lat1 = 47o26'56"N long1 = 122o18'34"W
			   EGLL lat2 = 51o28'39"N long2 = 000o27'41"W(=-0.4613889)
			*/

			Coordinate coord_ksea(Angle(47, 26, 56), Angle(-122, 18, 34), 0);
			Coordinate coord_egll(Angle(51, 28, 39), Angle(-0.4613889), 0);

			RelativePos relative_pos;
			coord_egll.get_relative_pos_to(coord_ksea, relative_pos);

			// rhumb line distance
			Assert::AreEqual(8809, int(relative_pos.dist_loxo));

			// the constant loxodrom heading shall be 267o5'8"
			Assert::AreEqual(267, relative_pos.heading_loxo.get_degree());
			Assert::AreEqual(5, relative_pos.heading_loxo.get_minute());
			Assert::AreEqual(8, relative_pos.heading_loxo.get_second());
		}

		TEST_METHOD(TestComputeRouteLoxodrom2)
		{
			// some navigation points near to LHBP
			Coordinate VETIK(Angle(47.352777778), Angle(20.232500000), 0);
			Coordinate UVERA(Angle(47.200000000), Angle(20.429722222), 0);
			Coordinate KEZAL(Angle(47.153611111), Angle(20.231388889), 0);
			Coordinate ABONY(Angle(47.270833333), Angle(19.979166667), 0);

			RelativePos relative_pos;

			// VETIK --> KEZAL : N --> S (180)
			VETIK.get_relative_pos_to(KEZAL, relative_pos);
			Assert::AreEqual(180, relative_pos.heading_loxo.get_degree());

			// KEZAL --> VETIK : S --> N (0)
			KEZAL.get_relative_pos_to(VETIK, relative_pos);
			Assert::AreEqual(0, relative_pos.heading_loxo.get_degree());

			// VETIK --> ABONY : SW (244)
			VETIK.get_relative_pos_to(ABONY, relative_pos);
			Assert::AreEqual(244, relative_pos.heading_loxo.get_degree());

			// ABONY --> VETIK : NE (64)
			ABONY.get_relative_pos_to(VETIK, relative_pos);
			Assert::AreEqual(64, relative_pos.heading_loxo.get_degree());

			// VETIK --> UEVRA : SE (138)
			VETIK.get_relative_pos_to(UVERA, relative_pos);
			Assert::AreEqual(138, relative_pos.heading_loxo.get_degree());

			// UVERA --> VETIK : NW (318)
			UVERA.get_relative_pos_to(VETIK, relative_pos);
			Assert::AreEqual(318, relative_pos.heading_loxo.get_degree());
		}

		TEST_METHOD(TestCoordinateToStringDegMinSec)
		{
			Coordinate coord_ksea(Angle(47, 26, 56), Angle(122, 18, 34), 0);
			GlobalOptions::get_instance()->set_option(OPTION_KEY_ANGLE_FORMAT, "ANGLE_DEG_MIN_SEC");

			std::string coord_str = coord_ksea.to_string();
			Assert::AreEqual("N47ø26\'56\" E122ø18\'34\"",coord_str.c_str());
		}

		TEST_METHOD(TestCoordinateToStringDegDecMin)
		{
			Coordinate coord_ksea(Angle(47, 26, 56), Angle(122, 18, 34), 0);
			GlobalOptions::get_instance()->set_option(OPTION_KEY_ANGLE_FORMAT, "ANGLE_DEG_DECMIN");

			std::string coord_str = coord_ksea.to_string();
			Assert::AreEqual("N47ø26.9\' E122ø18.5\'", coord_str.c_str());
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
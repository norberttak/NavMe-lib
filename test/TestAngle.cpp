#include "CppUnitTest.h"
#include "NavMeLib.h"
#include "Logger.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace test
{
	TEST_CLASS(TestAngle)
	{
	private:
		//
	public:
		TEST_METHOD_INITIALIZE(TestAngleInit)
		{
			
		}

		TEST_METHOD(TestDegree1)
		{
			// 18o30'0'' = 18.5

			Angle angle(18.5);
			Assert::AreEqual(18, angle.get_degree());
			Assert::AreEqual(30, angle.get_minute());
			Assert::AreEqual(0, angle.get_second());
		}

		TEST_METHOD(TestDegree2)
		{
			// 18o33'36'' = 18.56o

			Angle angle(18, 33, 36);
			Assert::AreEqual(18, angle.get_degree());
			Assert::AreEqual(33, angle.get_minute());
			Assert::AreEqual(36, angle.get_second());
		}

		TEST_METHOD(TestDegree3)
		{
			// 18o33.6' = 18o33'36''

			Angle angle(18, 33.6);
			Assert::AreEqual(18, angle.get_degree());
			Assert::AreEqual(33, angle.get_minute());
			Assert::AreEqual(36, angle.get_second());
		}

		TEST_METHOD(TestAngleEqual)
		{
			Angle angle1(-12.23);
			Angle angle2(-12.23);

			Assert::IsTrue(angle1 == angle2);
			Assert::IsFalse(angle1 != angle2);
		}

		TEST_METHOD(TestAnglePlus)
		{
			Angle angle1(11,22,33);
			Angle angle2(1.5);

			angle1 = angle1 + angle2;

			Assert::AreEqual(11+1, angle1.get_degree());
			Assert::AreEqual(22+30, angle1.get_minute());
			Assert::AreEqual(33+0, angle1.get_second());
		}

		TEST_METHOD(TestAngleMinus)
		{
			Angle angle1(-11, 22, 33);
			Angle angle2(1);

			angle1 = angle1 - angle2;

			Assert::AreEqual(-11 - 1, angle1.get_degree());
			Assert::AreEqual(22, angle1.get_minute());
			Assert::AreEqual(33, angle1.get_second());
		}

		TEST_METHOD(TestAngleString)
		{
			GlobalOptions::get_instance()->set_option(OPTION_KEY_ANGLE_FORMAT, "ANGLE_DEG_DECMIN");
			Angle angle(-5, 0, 00);
			Assert::AreEqual("-5ø00.0'", angle.to_string(false).c_str());

			GlobalOptions::get_instance()->set_option(OPTION_KEY_ANGLE_FORMAT, "ANGLE_DEG_DECMIN");
			Angle angle1(125, 43, 21);
			Assert::AreEqual("125ø43.3'", angle1.to_string(false).c_str());

			Angle angle2(-11, 1, 30);
			Assert::AreEqual("-11ø01.5'", angle2.to_string(false).c_str());
		}

		TEST_METHOD(TestAngleAssignDouble)
		{
			Angle angle(1, 2, 3);

			// assign a double value to the Angle object
			angle = 100.5;
			Assert::AreEqual(100, angle.get_degree());
			Assert::AreEqual(30, angle.get_minute());
			Assert::AreEqual(0, angle.get_second());
		}

		TEST_METHOD(TestAngleToStringDegMinSec)
		{
			GlobalOptions::get_instance()->set_option(OPTION_KEY_ANGLE_FORMAT, "ANGLE_DEG_MIN_SEC");
			Angle angle(18, 30, 40);
			std::string str = angle.to_string(false);
			Assert::AreEqual("18ø30'40\"", str.c_str());
		}

		TEST_METHOD(TestAngleToStringDouble)
		{
			GlobalOptions::get_instance()->set_option(OPTION_KEY_ANGLE_FORMAT, "ANGLE_DOUBLE");
			Angle angle(-20, 30, 0);
			std::string str = angle.to_string(false);
			Assert::AreEqual("-20.5", str.c_str());
		}

		TEST_METHOD_CLEANUP(TestAngleCleanup)
		{
		
		}

	};
}

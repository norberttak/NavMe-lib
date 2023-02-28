#include <iostream>
#include "CppUnitTest.h"
#include "NavMeLib.h"
#include "Logger.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace test
{
	TEST_CLASS(TestGlobalOptions)
	{
	private:
		//
	public:
		TEST_METHOD_INITIALIZE(TestGlobalOptionsInit)
		{

		}

		TEST_METHOD(TestSetGetOption)
		{
			GlobalOptions::get_instance()->set_option(OPTION_KEY_ANGLE_FORMAT, "ANGLE_DEG_MIN_SEC");
			std::string angle_format;
			GlobalOptions::get_instance()->get_option(OPTION_KEY_ANGLE_FORMAT, angle_format);
			Assert::AreEqual("ANGLE_DEG_MIN_SEC", angle_format.c_str());
		}

		TEST_METHOD(TestGetNoneExistingOption)
		{
			std::string option_value = "x";
			bool result = GlobalOptions::get_instance()->get_option("kutya_fule", option_value);
			Assert::IsFalse(result);
			Assert::AreEqual(std::string("x"), option_value);
		}
	
		TEST_METHOD(TestSaveAndLoadFile)
		{
			std::string option_value = "100";
			std::string option_name = "option1";

			GlobalOptions::get_instance()->set_option(option_name, option_value);

			// set option value and save it into a file
			std::ostringstream file_name_str;
			file_name_str << "test_otions_" << rand() << ".ini";
			GlobalOptions::get_instance()->set_option(option_name, option_value);
			GlobalOptions::get_instance()->save_options_to_file(file_name_str.str());

			GlobalOptions::get_instance()->set_option(option_name, option_value+"1");
			GlobalOptions::get_instance()->load_options_from_file(file_name_str.str());

			std::string option_value_read_from_file;
			GlobalOptions::get_instance()->get_option(option_name, option_value_read_from_file);
			// the option value shall be restored from the previously saved states

			Assert::AreEqual(option_value, option_value_read_from_file);
		}

		TEST_METHOD_CLEANUP(TestGlobalOptionsCleanup)
		{

		}

	};
}

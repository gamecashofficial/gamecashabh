#include <boost/algorithm/string.hpp>
#include <boost/foreach.hpp>
#include <boost/test/unit_test.hpp>

#include "util.h"

BOOST_AUTO_TEST_SUITE(getarg_tests)

static void
ResetArgs(const std::string& strArg)
{
    std::vector<std::string> vecArg;
    boost::split(vecArg, strArg, boost::is_space(), boost::token_compress_on);

    // Insert dummy executable name:
    vecArg.insert(vecArg.begin(), "testbitcoin");

    // Convert to char*:
    std::vector<const char*> vecChar;
    BOOST_FOREACH(std::string& s, vecArg)
        vecChar.push_back(s.c_str());

    ParseParameters(vecChar.size(), &vecChar[0]);
}

BOOST_AUTO_TEST_CASE(boolarg)
{
    ResetArgs("-GCSH");
    BOOST_CHECK(GetBoolArg("-GCSH"));
    BOOST_CHECK(GetBoolArg("-GCSH", false));
    BOOST_CHECK(GetBoolArg("-GCSH", true));

    BOOST_CHECK(!GetBoolArg("-fo"));
    BOOST_CHECK(!GetBoolArg("-fo", false));
    BOOST_CHECK(GetBoolArg("-fo", true));

    BOOST_CHECK(!GetBoolArg("-GCSHo"));
    BOOST_CHECK(!GetBoolArg("-GCSHo", false));
    BOOST_CHECK(GetBoolArg("-GCSHo", true));

    ResetArgs("-GCSH=0");
    BOOST_CHECK(!GetBoolArg("-GCSH"));
    BOOST_CHECK(!GetBoolArg("-GCSH", false));
    BOOST_CHECK(!GetBoolArg("-GCSH", true));

    ResetArgs("-GCSH=1");
    BOOST_CHECK(GetBoolArg("-GCSH"));
    BOOST_CHECK(GetBoolArg("-GCSH", false));
    BOOST_CHECK(GetBoolArg("-GCSH", true));

    // New 0.6 feature: auto-map -nosomething to !-something:
    ResetArgs("-noGCSH");
    BOOST_CHECK(!GetBoolArg("-GCSH"));
    BOOST_CHECK(!GetBoolArg("-GCSH", false));
    BOOST_CHECK(!GetBoolArg("-GCSH", true));

    ResetArgs("-noGCSH=1");
    BOOST_CHECK(!GetBoolArg("-GCSH"));
    BOOST_CHECK(!GetBoolArg("-GCSH", false));
    BOOST_CHECK(!GetBoolArg("-GCSH", true));

    ResetArgs("-GCSH -noGCSH");  // -GCSH should win
    BOOST_CHECK(GetBoolArg("-GCSH"));
    BOOST_CHECK(GetBoolArg("-GCSH", false));
    BOOST_CHECK(GetBoolArg("-GCSH", true));

    ResetArgs("-GCSH=1 -noGCSH=1");  // -GCSH should win
    BOOST_CHECK(GetBoolArg("-GCSH"));
    BOOST_CHECK(GetBoolArg("-GCSH", false));
    BOOST_CHECK(GetBoolArg("-GCSH", true));

    ResetArgs("-GCSH=0 -noGCSH=0");  // -GCSH should win
    BOOST_CHECK(!GetBoolArg("-GCSH"));
    BOOST_CHECK(!GetBoolArg("-GCSH", false));
    BOOST_CHECK(!GetBoolArg("-GCSH", true));

    // New 0.6 feature: treat -- same as -:
    ResetArgs("--GCSH=1");
    BOOST_CHECK(GetBoolArg("-GCSH"));
    BOOST_CHECK(GetBoolArg("-GCSH", false));
    BOOST_CHECK(GetBoolArg("-GCSH", true));

    ResetArgs("--noGCSH=1");
    BOOST_CHECK(!GetBoolArg("-GCSH"));
    BOOST_CHECK(!GetBoolArg("-GCSH", false));
    BOOST_CHECK(!GetBoolArg("-GCSH", true));

}

BOOST_AUTO_TEST_CASE(stringarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-GCSH", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-GCSH", "eleven"), "eleven");

    ResetArgs("-GCSH -GCSH");
    BOOST_CHECK_EQUAL(GetArg("-GCSH", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-GCSH", "eleven"), "");

    ResetArgs("-GCSH=");
    BOOST_CHECK_EQUAL(GetArg("-GCSH", ""), "");
    BOOST_CHECK_EQUAL(GetArg("-GCSH", "eleven"), "");

    ResetArgs("-GCSH=11");
    BOOST_CHECK_EQUAL(GetArg("-GCSH", ""), "11");
    BOOST_CHECK_EQUAL(GetArg("-GCSH", "eleven"), "11");

    ResetArgs("-GCSH=eleven");
    BOOST_CHECK_EQUAL(GetArg("-GCSH", ""), "eleven");
    BOOST_CHECK_EQUAL(GetArg("-GCSH", "eleven"), "eleven");

}

BOOST_AUTO_TEST_CASE(intarg)
{
    ResetArgs("");
    BOOST_CHECK_EQUAL(GetArg("-GCSH", 11), 11);
    BOOST_CHECK_EQUAL(GetArg("-GCSH", 0), 0);

    ResetArgs("-GCSH -GCSH");
    BOOST_CHECK_EQUAL(GetArg("-GCSH", 11), 0);
    BOOST_CHECK_EQUAL(GetArg("-GCSH", 11), 0);

    ResetArgs("-GCSH=11 -GCSH=12");
    BOOST_CHECK_EQUAL(GetArg("-GCSH", 0), 11);
    BOOST_CHECK_EQUAL(GetArg("-GCSH", 11), 12);

    ResetArgs("-GCSH=NaN -GCSH=NotANumber");
    BOOST_CHECK_EQUAL(GetArg("-GCSH", 1), 0);
    BOOST_CHECK_EQUAL(GetArg("-GCSH", 11), 0);
}

BOOST_AUTO_TEST_CASE(doubledash)
{
    ResetArgs("--GCSH");
    BOOST_CHECK_EQUAL(GetBoolArg("-GCSH"), true);

    ResetArgs("--GCSH=verbose --GCSH=1");
    BOOST_CHECK_EQUAL(GetArg("-GCSH", ""), "verbose");
    BOOST_CHECK_EQUAL(GetArg("-GCSH", 0), 1);
}

BOOST_AUTO_TEST_CASE(boolargno)
{
    ResetArgs("-noGCSH");
    BOOST_CHECK(!GetBoolArg("-GCSH"));
    BOOST_CHECK(!GetBoolArg("-GCSH", true));
    BOOST_CHECK(!GetBoolArg("-GCSH", false));

    ResetArgs("-noGCSH=1");
    BOOST_CHECK(!GetBoolArg("-GCSH"));
    BOOST_CHECK(!GetBoolArg("-GCSH", true));
    BOOST_CHECK(!GetBoolArg("-GCSH", false));

    ResetArgs("-noGCSH=0");
    BOOST_CHECK(GetBoolArg("-GCSH"));
    BOOST_CHECK(GetBoolArg("-GCSH", true));
    BOOST_CHECK(GetBoolArg("-GCSH", false));

    ResetArgs("-GCSH --noGCSH");
    BOOST_CHECK(GetBoolArg("-GCSH"));

    ResetArgs("-noGCSH -GCSH"); // GCSH always wins:
    BOOST_CHECK(GetBoolArg("-GCSH"));
}

BOOST_AUTO_TEST_SUITE_END()

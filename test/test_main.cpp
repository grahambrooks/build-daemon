#define BOOST_TEST_MAIN
#ifndef MAKEFILE_BUILD
// Work around for xcode's default to always link against dynamic libraries and the need to static linking for distribution.
#define BOOST_TEST_DYN_LINK
#endif
#define BOOST_TEST_MODULE BuildDaemonTests

#include <boost/test/unit_test.hpp>


BOOST_AUTO_TEST_CASE(holding_pattern) {
  
  BOOST_CHECK(true);
}

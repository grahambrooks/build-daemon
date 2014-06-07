#include <boost/test/unit_test.hpp>
#include "command_line.hpp"

BOOST_AUTO_TEST_CASE(recognises_verbose_options) {
  BOOST_CHECK(is::not_null(""));
  BOOST_CHECK(is::switch_char('-'));
  BOOST_CHECK(is::verbose_char('v'));
  BOOST_CHECK(is::eos(""));
  BOOST_CHECK(is::verbose_option("-v"));
}

BOOST_AUTO_TEST_CASE(recognises_tigger_option) {
  BOOST_CHECK(is::trigger_option("-t"));
}

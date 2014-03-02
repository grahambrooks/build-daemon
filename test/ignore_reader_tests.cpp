#include <boost/test/unit_test.hpp>

#include "ignore_list_reader.hpp"

BOOST_AUTO_TEST_CASE(blank_lines_are_ignored) {
  std::stringstream ss("\n\n\n\n\n\n\n");

  auto ignore_list = ignore_list_reader::read(ss);

  BOOST_CHECK_EQUAL(0, ignore_list.size());
}

BOOST_AUTO_TEST_CASE(handles_empty_files) {
  std::stringstream ss("");

  auto ignore_list = ignore_list_reader::read(ss);

  BOOST_CHECK_EQUAL(0, ignore_list.size());
}


BOOST_AUTO_TEST_CASE(accepts_simple_text_pattern) {
  std::stringstream ss("simple.txt");

  auto ignore_list = ignore_list_reader::read(ss);

  BOOST_CHECK_EQUAL(1, ignore_list.size());
}

BOOST_AUTO_TEST_CASE(ignores_commented_lines) {
  std::stringstream ss("# this is a comment");

  auto ignore_list = ignore_list_reader::read(ss);

  BOOST_CHECK_EQUAL(0, ignore_list.size());
}

#include <boost/test/unit_test.hpp>
#include "build_tool_selector.hpp"

BOOST_AUTO_TEST_CASE(matches_makefile) {
  class tool_tester_mock : public tool_tester {
    virtual bool test(const char* filename) {
      return strcmp(filename, "makefile") == 0;
    }
  };

  tool_tester_mock mock;

  tool_selector selector;

  BOOST_CHECK_EQUAL(selector.select(mock), "make");
}

BOOST_AUTO_TEST_CASE(matches_ant_build_files) {
  class tool_tester_mock : public tool_tester {
    virtual bool test(const char* filename) {
      return strcmp(filename, "build.xml") == 0;
    }
  };

  tool_tester_mock mock;

  tool_selector selector;

  BOOST_CHECK_EQUAL(selector.select(mock), "ant");
}

BOOST_AUTO_TEST_CASE(matches_first_available_build_file) {
  class tool_tester_mock : public tool_tester {
    virtual bool test(const char* filename) {
      return strcmp(filename, "build.xml") == 0 || strcmp(filename, "makefile") == 0;
    }
  };

  tool_tester_mock mock;

  tool_selector selector;

  BOOST_CHECK_EQUAL(selector.select(mock), "make");
}

BOOST_AUTO_TEST_CASE(matches_maven_pom_files) {
  class tool_tester_mock : public tool_tester {
    virtual bool test(const char* filename) {
      return strcmp(filename, "pom.xml") == 0;
    }
  };

  tool_tester_mock mock;

  tool_selector selector;

  BOOST_CHECK_EQUAL(selector.select(mock), "mvn");
}

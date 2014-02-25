#include <boost/test/unit_test.hpp>

#include "filters.hpp"


BOOST_AUTO_TEST_CASE(hidden_file_filter_filters_paths_that_start_with_a_dot) {
  filters filters;

  BOOST_CHECK(filters.hidden_file_filter("/some/path/.git/changed_file", 0));
  BOOST_CHECK(filters.hidden_file_filter("/some/path/.#emacs_temp_file", 0));

}

BOOST_AUTO_TEST_CASE(filters_events_on_symbolic_links) {

  filters filters;

  BOOST_CHECK(filters.no_symbolic_links("", kFSEventStreamEventFlagItemIsSymlink));
}

BOOST_AUTO_TEST_CASE(filters_in_create_events) {

  filters filters;

  BOOST_CHECK(!filters.no_symbolic_links("", kFSEventStreamEventFlagItemCreated));
  BOOST_CHECK(!filters.no_symbolic_links("", kFSEventStreamEventFlagItemRemoved));
  BOOST_CHECK(!filters.no_symbolic_links("", kFSEventStreamEventFlagItemRenamed));
  BOOST_CHECK(!filters.no_symbolic_links("", kFSEventStreamEventFlagItemModified));
}

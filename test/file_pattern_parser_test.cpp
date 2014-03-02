#include <boost/test/unit_test.hpp>
#include <string>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>

using namespace std;

class file_pattern_parser {
public:
  static boost::regex parse(const char* text) {

    auto escape_chars = "^.$|()[]*+?";

    string s =  boost::filesystem::current_path().string() + "/";
    while (*text) {
      if (*text == '/' && text[1] == 0) {
	s = s.append("(/.*)?");

	text++;
      } else if (text[0] == '*' && text[1] == '*' && text[2] == '/') {
	s = s.append("(.*/)*");
	text += 3;
      } else {
	if (strchr(escape_chars, *text) != NULL) {
	  s = s.append("\\");
	}
	s.push_back(*text++);
      }
    }
    return boost::regex(s);
  }
};

BOOST_AUTO_TEST_CASE(parses_simple_names) {
  const char* text = "somename";

  auto re = file_pattern_parser::parse(text);
  
  auto expected = (boost::filesystem::current_path() / "somename").string();

  BOOST_CHECK(boost::regex_match(expected, re));
}

BOOST_AUTO_TEST_CASE(matches_paths_for_path_suffix) {
  const char* text = "somename/";

  auto re = file_pattern_parser::parse(text);
  
  auto expected = (boost::filesystem::current_path() / "somename").string();

  BOOST_CHECK(boost::regex_match(expected, re));
  BOOST_CHECK(boost::regex_match(expected + "/file", re));
  BOOST_CHECK(boost::regex_match(expected + "/path/to/file", re));
}

BOOST_AUTO_TEST_CASE(matches_names_for_regex_special_chars) {
  const char* text = "^.$|()[]*+?";

  auto re = file_pattern_parser::parse(text);

  auto expected = (boost::filesystem::current_path() / text).string();

  BOOST_CHECK(boost::regex_match(expected, re));

}

BOOST_AUTO_TEST_CASE(matches_path_elements) {
  const char* text = "**/somename";

  auto re = file_pattern_parser::parse(text);

  cout << "Regex " << re << endl;
  
  auto expected = (boost::filesystem::current_path() / "somename").string();

  BOOST_CHECK(boost::regex_match(expected, re));
  BOOST_CHECK(boost::regex_match((boost::filesystem::current_path() / "path" / "to" / "element" / "somename").string(), re));
}

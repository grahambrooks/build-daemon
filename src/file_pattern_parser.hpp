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


#include <string>
#include <vector>
#include <sys/syslimits.h>
using namespace std;

class ignore_list_reader {
public:
  static vector<string> read(istream &ss) {
    vector<string> result;

    char pattern[1025];

    while (ss.getline(pattern, 1025)) {
      if (strlen(pattern) > 0 && pattern[0] != '#') {
	result.push_back(pattern);
      }
    }
    return result;
  }
};

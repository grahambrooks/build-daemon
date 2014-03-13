#include <boost/filesystem.hpp>
#include <fstream>
#include "ignore_list_reader.hpp"
#include "file_pattern_parser.hpp"
#include "filters.hpp"


namespace lazy {
  namespace filesystem {
    vector<event_filter> path_event_filter::load(const string& path) {
      vector<event_filter> result;

      if (boost::filesystem::exists(path) && boost::filesystem::is_regular_file(path)) {
	ignore_list_reader reader;
	
	ifstream f;
	
	f.open(path.c_str());
	auto patterns = reader.read(f);
	f.close();

	for (auto &x : patterns) {
	  result.insert(result.end(), path_event_filter(file_pattern_parser::parse(x.c_str())));
	}
      }
      
      return result;
    }
  }

}

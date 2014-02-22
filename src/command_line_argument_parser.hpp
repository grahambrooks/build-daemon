#pragma once

#include <boost/program_options.hpp>
#include <vector>

using namespace std;

class arguments {
  constexpr static auto version_option_name = "verbose";
  constexpr static auto version_option = "verbose,v";
  constexpr static auto files_option_name = "input-files";

  boost::program_options::variables_map variables;

  friend class command_line_argument_parser;

public:
  arguments(boost::program_options::variables_map variables);
  
  bool version();
  
  const vector<string> parameters();
};

class command_line_argument_parser {
  boost::program_options::options_description desc;
public:
  command_line_argument_parser();

  arguments parse(int argc, const char *argv[]);
};

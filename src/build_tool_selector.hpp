#pragma once
#include <vector>

class tool_tester {
public:
  virtual bool test(const char* filename) = 0;
};

class tool_selector {
  std::vector< std::pair<const char*, const char *> > tools;
  
public:

  tool_selector() : tools{{ 
      { "makefile", "make" },
      { "Makefile", "make" },
      { "rakefile", "rake" },
      { "build.xml", "ant" },
      { "pom.xml",   "mvn" },
    }} {}
  
  const char * select(tool_tester& tester) {
    for (auto test : tools) {
      if (tester.test(test.first)) {
	return test.second;
      }
    }
    return "make";
  }
};

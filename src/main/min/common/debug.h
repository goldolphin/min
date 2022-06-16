//
// Created by goldolphin on 2022/6/9.
//

#pragma once

#include <iostream>

namespace min {

#ifdef NDEBUG
#define DEBUG_LOG(msg) ((void)0)
#else
#define DEBUG_LOG(msg) ({ DebugLogger::stream() << "                                                |" << msg; ((void)0); }) // NOLINT(bugprone-macro-parentheses)
#endif

class DebugLogger {
 private:
  DebugLogger() {
    std::cout << "                                                |## Debug flag: ON" << std::endl;
  }

 public:
  static std::ostream& stream() {
    static DebugLogger instance;
    return std::cout;
  }
};


}

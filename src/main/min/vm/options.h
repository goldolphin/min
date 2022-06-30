//
// Created by goldolphin on 2022/6/9.
//

#pragma once

#include <chrono>
#include <string>

namespace min {

struct HeapOptions {
  size_t capacity;
  double collect_threshold;
  std::chrono::milliseconds min_collect_interval;
  std::string reserved;

  static HeapOptions Default() {
    return {1024 * 1024, 0.8, std::chrono::milliseconds(10)};
  }
};

struct Options {
  HeapOptions heap_options;
  std::string reserved;

  static Options Default() {
    return { HeapOptions::Default() };
  };
};

}
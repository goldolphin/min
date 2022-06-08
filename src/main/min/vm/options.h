//
// Created by goldolphin on 2022/6/9.
//

#pragma once

#include <chrono>

namespace min {

struct HeapOptions {
  size_t capacity;
  double collect_threshold;
  std::chrono::milliseconds min_collect_interval;

  static HeapOptions Default() {
    return {1024 * 1024, 0.8, std::chrono::milliseconds(10)};
  }
};

struct Options {
  HeapOptions heap_options;

  static Options Default() {
    return { HeapOptions::Default() };
  };
};

}
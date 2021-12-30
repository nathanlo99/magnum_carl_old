
#pragma once

#include <string>
#include <vector>

static inline std::vector<std::string> split(std::string text,
                                             const std::string &pattern) {
  size_t pos = 0;
  std::vector<std::string> result;
  while ((pos = text.find(pattern)) != std::string::npos) {
    result.push_back(text.substr(0, pos));
    text.erase(0, pos + pattern.size());
  }
  result.push_back(text);
  return result;
}

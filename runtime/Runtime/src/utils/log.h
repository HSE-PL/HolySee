#pragma once
#include <iostream>

#ifdef DEBUG
#define logezhe std::cout
#else
class NullStream {
public:
  template <typename T>
  NullStream& operator<<(const T&) {
    return *this;
  }
  NullStream& operator<<(std::ostream& (*)(std::ostream&)) {
    return *this;
  }
};
static NullStream logezhe;
#endif

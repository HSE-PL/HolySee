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
};
static NullStream logezhe;
#endif
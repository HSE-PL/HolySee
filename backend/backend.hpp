#pragma once

#include <stdexcept>

class NotImplemented : public std::logic_error {
public:
  NotImplemented(std::string error_msg)
      : std::logic_error("Function not yet implemented" + error_msg) {};
};

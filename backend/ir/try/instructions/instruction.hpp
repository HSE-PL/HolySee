#pragma once

#include "../value.hpp"
#include <list>
#include <memory>

class Instruction {
protected:
  using vptr = std::shared_ptr<Value>;
  using CAItt = std::list<std::shared_ptr<Value>>::const_iterator;
  using AItt = std::list<std::shared_ptr<Value>>::iterator;
  std::shared_ptr<Value> dest_;
  std::list<std::shared_ptr<Value>> args;
  ValType vtype;

public:
  CAItt cbegin() const { return args.cbegin(); }
  CAItt cend() const { return args.cend(); }
  AItt begin() { return args.begin(); }
  AItt end() { return args.end(); }
  ValType type() const { return vtype; }
  /*std::shared_ptr<Value> dest() { return dest_; }*/
};

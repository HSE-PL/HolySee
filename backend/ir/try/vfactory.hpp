#pragma once

#include "value.hpp"
#include <memory>

class VFactory {
  using vptr = std::shared_ptr<Value>;

public:
  static vptr createInt(int i) { return std::make_shared<Int>(i); }
  static vptr createBool(bool b) { return std::make_shared<Bool>(b); }
  static vptr createLabel(std::string label) {
    return std::make_shared<Label>(label);
  }
  static vptr createUnit() { return std::make_shared<Unit>(); }
  static vptr createRef(Type t, std::string name) {
    return std::make_shared<Ref>(t, name);
  }
  static vptr createIntRef(std::string name) {
    return std::make_shared<Ref>(Type::Int, name);
  }
  static vptr createBoolRef(std::string name) {
    return std::make_shared<Ref>(Type::Bool, name);
  }
};

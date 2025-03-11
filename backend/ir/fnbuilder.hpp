#pragma once

#include "blocks/bblock.hpp"
#include "factory/vfactory.hpp"
#include "fn.hpp"
#include <memory>
// I probably should have builder ierarchy but i cba a little currently.
// might refactor it later.

class FnBuilder {
  using vptr = std::shared_ptr<Value>;
  Fn *fn;

public:
  FnBuilder(Type t, std::string name) { fn = new Fn(t, name); }
  ~FnBuilder() { delete fn; }
  void reset(Type t, std::string name) {
    delete fn;
    fn = new Fn(t, name);
  }
  std::shared_ptr<Fn> build() { return std::make_shared<Fn>(*fn); }
  FnBuilder &setName(std::string name) {
    fn->name_ = name;
    return *this;
  }
  FnBuilder &setType(Type t) {
    fn->type_ = t;
    return *this;
  }
  FnBuilder &addBlock(std::shared_ptr<BBlock> b) {
    fn->addBlock(b);
    return *this;
  }
  FnBuilder &addArg(Type t, std::string name) {
    auto ref = VFactory::createRef(t, name);
    fn->addArg(ref);
    return *this;
  }
};

#pragma once

#include "blocks/bblock.hpp"
#include "value.hpp"
#include <memory>
#include <string>

class Fn {
  friend class FnBuilder;
  using vptr = std::shared_ptr<Value>;
  using CBItt = std::list<std::shared_ptr<BBlock>>::const_iterator;
  using BItt = std::list<std::shared_ptr<BBlock>>::iterator;
  Type type_;
  std::string name;
  std::list<std::shared_ptr<BBlock>> blocks;
  std::list<vptr> args;
  Fn(Type type_, std::string name) : type_(type_), name(name) {}
  Fn(const Fn &other) = default;
  void addBlock(std::shared_ptr<BBlock> b) { blocks.push_back(b); }
  void addArg(vptr arg) { args.push_back(arg); }

public:
  Type type() { return type_; }
  CBItt cbegin() const { return blocks.cbegin(); }
  CBItt cend() const { return blocks.cend(); }
  BItt begin() { return blocks.begin(); }
  BItt end() { return blocks.end(); }
};

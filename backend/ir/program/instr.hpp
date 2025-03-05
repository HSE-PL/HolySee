#pragma once

#include "../types/itype.hpp"
#include "../types/vtype.hpp"
#include <iostream>
#include <optional>
#include <utility>
#include <variant>
#include <vector>

class BranchArgs {
  using Branches = std::tuple<std::string, std::string, std::string>;
  Branches args;

public:
  BranchArgs() = default;
  BranchArgs(Branches branches) : args(branches) {}
  BranchArgs(const BranchArgs &other) = default;
  BranchArgs &operator=(const BranchArgs &other) = default;
  BranchArgs &operator=(BranchArgs &&other) {
    std::swap(args, other.args);
    return *this;
  }
  BranchArgs(BranchArgs &&other) { std::swap(args, other.args); }
  std::string &cond() { return std::get<0>(args); }
  std::string ccond() const { return std::get<0>(args); }
  std::string &trueb() { return std::get<1>(args); }
  std::string ctrueb() const { return std::get<1>(args); }
  std::string &falseb() { return std::get<2>(args); }
  std::string cfalseb() const { return std::get<2>(args); }
};

class Val {
  using VarArg = std::vector<std::string>;
  using FnCall = std::pair<std::string, std::vector<std::string>>;
  using Args = std::vector<std::string>;
  using RefPair = std::pair<std::string, std::string>;
  using Value =
      std::variant<RefPair, BranchArgs, std::string, VarArg, FnCall, int>;
  VType type_;
  Value val_;

public:
  Val() = default;
  Val(VType t, Value val) : type_(t), val_(val) {}
  Val(const Val &other) = default;
  Val &operator=(const Val &other) = default;
  Val &operator=(Val &&other) {
    std::swap(val_, other.val_);
    std::swap(type_, other.type_);
    return *this;
  }
  Val(Val &&other) {
    std::swap(val_, other.val_);
    std::swap(type_, other.type_);
  }
  VType type() const { return type_; }
  Value val() const { return val_; }
  RefPair crefp() const { return std::get<0>(val_); }
  RefPair &refp() { return std::get<0>(val_); }
  BranchArgs cbranches() const { return std::get<1>(val_); }
  BranchArgs &branches() { return std::get<1>(val_); }
  std::string clabel() const { return std::get<2>(val_); }
  std::string &label() { return std::get<2>(val_); }
  std::string cref() const { return std::get<2>(val_); }
  std::string &ref() { return std::get<2>(val_); }
  Args cargs() const { return std::get<3>(val_); }
  Args &args() { return std::get<3>(val_); }
  FnCall &fncall() { return std::get<4>(val_); }
  FnCall cfncall() const { return std::get<4>(val_); }
  int cnumber() const { return std::get<5>(val_); }
  int &number() { return std::get<5>(val_); }
};

class Instr {
public:
  IType op;
  VType type;
  std::optional<std::string> dest;
  Val vals;

  friend std::ostream &operator<<(std::ostream &o, const Instr &fn);
};

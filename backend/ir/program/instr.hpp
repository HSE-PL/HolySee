#pragma once

#include "../types/itype.hpp"
#include "../types/vtype.hpp"
#include <iostream>
#include <optional>
#include <utility>
#include <variant>
#include <vector>

class Val {
  using Branches = std::pair<std::string, std::string>;
  using VarArg = std::vector<std::string>;
  using Value = std::variant<std::pair<std::string, std::string>, std::string,
                             std::vector<std::string>, int>;
  using Args = std::vector<std::string>;
  using RefPair = std::pair<std::string, std::string>;
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
  Branches cbranches() const { return std::get<0>(val_); }
  Branches &branches() { return std::get<0>(val_); }
  std::string clabel() const { return std::get<1>(val_); }
  std::string &label() { return std::get<1>(val_); }
  std::string cref() const { return std::get<1>(val_); }
  std::string &ref() { return std::get<1>(val_); }
  Args cargs() const { return std::get<2>(val_); }
  Args &args() { return std::get<2>(val_); }
  int cnumber() const { return std::get<3>(val_); }
  int &number() { return std::get<3>(val_); }
};

class Instr {
public:
  IType op;
  VType type;
  std::optional<std::string> dest;
  Val vals;

  friend std::ostream &operator<<(std::ostream &o, const Instr &fn);
};

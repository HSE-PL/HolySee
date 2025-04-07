#pragma once

#include "placement.hpp"
#include <string>

enum class MType {
  i32,
  boolean,
  label,
};

class Mach {
  MType type_;

public:
  Mach(MType type_) : type_(type_) {}
  MType type() { return type_; }
  virtual std::string emit() = 0;
  virtual ~Mach() {}
};

class MValue : public Mach {

public:
  MValue(MType type_) : Mach(type_) {}

public:
};

class Imm : public MValue {
  int val_;

public:
  Imm(int val) : MValue(MType::i32), val_(val) {}
  virtual std::string emit() { return std::to_string(val_); }
  int val() { return val_; }
};

class MPlaced : public MValue {
  Placement place_;

public:
  MPlaced(Placement place_, MType type_) : MValue(type_), place_(place_) {}
  virtual std::string emit() { return place_.emit(); }
  Placement place() { return place_; }
};

class MInt : public MPlaced {
public:
  MInt(Placement place) : MPlaced(place, MType::i32) {}
};

class MBool : public MPlaced {
public:
  MBool(Placement place) : MPlaced(place, MType::boolean) {}
};

class MLabel : public Mach {
  std::string label_;

public:
  std::string label() { return label_; }
  virtual std::string emit() { return label_ + ":"; }
  MLabel(std::string str) : Mach(MType::label), label_(str) {}
};

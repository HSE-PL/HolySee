#pragma once

#include <string>

enum class ValType {
  Ref,
  Const,
  Label,
  Nothing,
};

enum class Type {
  Int,
  Bool,
  Unit,
};

class Value {
protected:
  ValType t;
  Value(ValType vt) : t(vt) {}

public:
  ValType type() const { return t; }
  virtual ~Value() = 0;
};

class Ref final : public Value {
  friend class VFactory;
  Type type_;
  std::string name_;
  Ref(Type t, std::string name) : Value(ValType::Ref), name_(name) {}

public:
  std::string name() { return name_; }
  Type vtype() { return type_; }
  virtual ~Ref() = default;
};

class Label final : public Value {
  friend class VFactory;
  std::string label_;
  Label(std::string label) : Value(ValType::Label), label_(label) {}

public:
  std::string label() { return label_; }
  virtual ~Label() = default;
};

/*class FCall final : public Value {*/
/*  std::string fname_;*/
/*  FCall(std::string fname) : Value(ValType::FCall), fname_(fname) {}*/
/**/
/*public:*/
/*  std::string fname() { return fname_; }*/
/*  virtual ~FCall() = default;*/
/*};*/

class Int final : public Value {
  friend class VFactory;
  int val_;
  Int(int val) : Value(ValType::Const), val_(val) {}

public:
  int val() { return val_; }
  virtual ~Int() = default;
};

class Bool final : public Value {
  friend class VFactory;
  bool val_;
  Bool(bool val) : Value(ValType::Const), val_(val) {}

public:
  bool val() { return val_; }
  virtual ~Bool() = default;
};

struct Unit final : public Value {
  Unit() : Value(ValType::Nothing) {}
};

#pragma once

#include "streamers/iostreamer.hpp"
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
  friend class IOStreamer;

protected:
  ValType t;

public:
  Value(ValType vt) : t(vt) {}
  ValType type() const { return t; }
  virtual void accept(IOStreamer &io) = 0;
  virtual ~Value() {}
};

class Ref final : public Value {
  friend class IOStreamer;
  Type type_;
  std::string name_;

public:
  Ref(Type t, std::string name) : Value(ValType::Ref), type_(t), name_(name) {}
  Ref(const Ref &other) = default;
  std::string name() { return name_; }
  Type vtype() { return type_; }
  virtual void accept(IOStreamer &io) { io.visit(*this); }
};

class Label final : public Value {
  friend class IOStreamer;
  std::string label_;

public:
  virtual void accept(IOStreamer &io) { io.visit(*this); }
  Label(std::string label) : Value(ValType::Label), label_(label) {}
  Label(const Label &other) = default;
  std::string label() { return label_; }
};

class Int final : public Value {
  friend class IOStreamer;
  int val_;

public:
  virtual void accept(IOStreamer &io) { io.visit(*this); }
  Int(int val) : Value(ValType::Const), val_(val) {}
  Int(const Int &other) = default;
  int val() { return val_; }
};

class Bool final : public Value {
  friend class IOStreamer;
  bool val_;

public:
  virtual void accept(IOStreamer &io) { io.visit(*this); }
  Bool(bool val) : Value(ValType::Const), val_(val) {}
  Bool(const Bool &other) = default;
  bool val() { return val_; }
};

struct Unit final : public Value {
  virtual void accept(IOStreamer &io) { io.visit(*this); }
  Unit() : Value(ValType::Nothing) {}
  Unit(const Unit &other) = default;
};

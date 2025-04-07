#pragma once

#include "../codegen/emit/mctx.hpp"
#include "streamers/iostreamer.hpp"
#include <memory>
#include <string>

class Mach;

using mach = std::shared_ptr<Mach>;

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
  friend class MCtx;
  virtual mach emit(MCtx &ctx) = 0;
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
  friend class MCtx;
  virtual mach emit(MCtx &ctx) { return ctx.visit(*this); }
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
  friend class MCtx;
  virtual mach emit(MCtx &ctx) { return ctx.visit(*this); }
  virtual void accept(IOStreamer &io) { io.visit(*this); }
  Label(std::string label) : Value(ValType::Label), label_(label) {}
  Label(const Label &other) = default;
  std::string label() { return label_; }
};

class Int final : public Value {
  friend class IOStreamer;
  int val_;

public:
  friend class MCtx;
  virtual mach emit(MCtx &ctx) { return ctx.visit(*this); }
  virtual void accept(IOStreamer &io) { io.visit(*this); }
  Int(int val) : Value(ValType::Const), val_(val) {}
  Int(const Int &other) = default;
  int val() { return val_; }
};

class Bool final : public Value {
  friend class IOStreamer;
  bool val_;

public:
  friend class MCtx;
  virtual mach emit(MCtx &ctx) { return ctx.visit(*this); }
  virtual void accept(IOStreamer &io) { io.visit(*this); }
  Bool(bool val) : Value(ValType::Const), val_(val) {}
  Bool(const Bool &other) = default;
  bool val() { return val_; }
};

struct Unit final : public Value {
  friend class MCtx;
  virtual mach emit(MCtx &ctx) { return ctx.visit(*this); }
  virtual void accept(IOStreamer &io) { io.visit(*this); }
  Unit() : Value(ValType::Nothing) {}
  Unit(const Unit &other) = default;
};

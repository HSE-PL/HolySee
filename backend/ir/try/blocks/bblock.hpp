#pragma once

#include "../instructions/instruction.hpp"
#include <memory>

class BBlock {
  friend class BBuilder;
  using CIItt = std::list<std::shared_ptr<Instruction>>::const_iterator;
  using IItt = std::list<std::shared_ptr<Instruction>>::iterator;
  std::string name;
  std::list<std::shared_ptr<Instruction>> instrs;
  BBlock(std::string name) : name(name) {}
  BBlock(std::string name, std::list<std::shared_ptr<Instruction>> instrs)
      : name(name), instrs(instrs) {}

  BBlock(const BBlock &other) = default;
  BBlock &operator=(const BBlock &other) = default;

  void addInstr(std::shared_ptr<Instruction> instr) { instrs.push_back(instr); }

public:
  CIItt cbegin() const { return instrs.cbegin(); }
  CIItt cend() const { return instrs.cend(); }
  IItt begin() { return instrs.begin(); }
  IItt end() { return instrs.end(); }
};

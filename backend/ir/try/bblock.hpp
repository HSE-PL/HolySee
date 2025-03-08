#pragma once

#include "instruction.hpp"
#include <memory>

class BBlock {
  friend class BBuilder;
  using CAItt = std::list<std::shared_ptr<Instruction>>::const_iterator;
  using AItt = std::list<std::shared_ptr<Instruction>>::iterator;
  std::string name;
  std::list<std::shared_ptr<Instruction>> instrs;
  BBlock(std::string name) : name(name) {}
  BBlock(std::string name, std::list<std::shared_ptr<Instruction>> instrs)
      : name(name), instrs(instrs) {}

  BBlock(const BBlock &other) = default;
  BBlock &operator=(const BBlock &other) = default;

  void addInstr(std::shared_ptr<Instruction> instr) { instrs.push_back(instr); }

public:
  CAItt cbegin() const { return instrs.cbegin(); }
  CAItt cend() const { return instrs.cend(); }
  AItt begin() { return instrs.begin(); }
  AItt end() { return instrs.end(); }
};

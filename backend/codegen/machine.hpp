#pragma once

#include "../ir/program/instr.hpp"

class MachineFunction;

class MachineBasicBlock;

class MachineInstr {
public:
  virtual ~MachineInstr() = 0;
};

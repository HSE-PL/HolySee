#pragma once

#include "instr.hpp"
#include <string>
#include <vector>

struct Block {
  std::string name;
  std::vector<Instr> instrs;
  Block(std::string name, std::vector<Instr> instrs)
      : name(name), instrs(instrs) {}
  Block(std::vector<Instr> instrs) : instrs(instrs) {}
  void add_instr(Instr instr) { instrs.push_back(instr); }
  friend std::ostream &operator<<(std::ostream &o, const Block &fn);
};

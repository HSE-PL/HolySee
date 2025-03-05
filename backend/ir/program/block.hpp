#pragma once

#include "instr.hpp"
#include <list>

struct Block {
  std::string name;
  std::list<Instr> instrs;
  Block(std::string name, std::list<Instr> instrs)
      : name(name), instrs(instrs) {}
  Block(std::list<Instr> instrs) : instrs(instrs) {}
  void add_instr(Instr instr) { instrs.push_back(instr); }
  friend std::ostream &operator<<(std::ostream &o, const Block &fn);
};

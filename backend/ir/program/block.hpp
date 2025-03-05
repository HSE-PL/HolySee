#pragma once

#include "instr.hpp"
#include <list>

struct Block {
  std::string name;
  size_t size = 0;
  std::list<Instr> instrs;
  Block(std::string name, std::list<Instr> instrs)
      : name(name), instrs(instrs) {}
  Block(std::list<Instr> instrs) : instrs(instrs) {}
  void add_instr(Instr instr) {
    instrs.push_back(instr);
    ++size;
  }
  friend std::ostream &operator<<(std::ostream &o, const Block &fn);
};

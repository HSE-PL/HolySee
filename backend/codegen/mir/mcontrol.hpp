#pragma once

#include "mvalue.hpp"
#include <list>
#include <memory>
#include <vector>

class MachineInstr : public Mach {
public:
  MachineInstr(MType tp) : Mach(tp) {}
};

class MachineBasicBlock : public Mach {
  std::string name;
  std::vector<std::shared_ptr<MachineInstr>> instrs;

public:
  virtual std::string emit() {
    std::string holder = name + ":" + "\n";
    for (auto &i : instrs) {
      holder += "  " + i->emit() + "\n";
    }
    return holder;
  }
  MachineBasicBlock(std::string name,
                    std::vector<std::shared_ptr<MachineInstr>> mi)
      : Mach(MType::label), name(name), instrs(mi) {}
};

class MachineFunction : public Mach {
  std::string name;
  std::vector<std::shared_ptr<MachineBasicBlock>> blocks;
  std::list<std::shared_ptr<MachineInstr>> prologue;
  std::list<std::shared_ptr<MachineInstr>> epilogue;

public:
  virtual std::string emit() {
    std::string holder = name + ":" + "\n";
    for (auto &p : prologue) {
      holder += "  " + p->emit() + "\n";
    }
    for (auto &b : blocks) {
      holder += b->emit();
    }
    for (auto &p : epilogue) {
      holder += "  " + p->emit() + "\n";
    }
    return holder;
  }
  MachineFunction(std::string name) : Mach(MType::label), name(name) {}
  void addPrologue(std::list<std::shared_ptr<MachineInstr>> prologue) {
    this->prologue = prologue;
  }
  void addEpilogue(std::list<std::shared_ptr<MachineInstr>> epilogue) {
    this->epilogue = epilogue;
  }
  void addBlock(std::shared_ptr<MachineBasicBlock> mbb) {
    blocks.push_back(mbb);
  }
};

class MachineExec : public Mach {
  std::vector<std::shared_ptr<MachineFunction>> fns;

public:
  virtual std::string emit() {
    std::string holder;
    for (auto &fn : fns) {
      holder += fn->emit();
    }
    return holder;
  }
  void addFunc(std::shared_ptr<MachineFunction> mf) { fns.push_back(mf); }
  MachineExec() : Mach(MType::label) {}
};

#include "test_pass.hpp"
#include "ir/ir.hpp"
#include <iostream>

using namespace std;

void TestPass::pass(Function &f) {
  for (auto &block : f.blocks) {
    for (auto &instr : block.instrs) {
      if (instr.op == IType::Br) {
        cout << "Br INSTRUCTION:" << endl;
        cout << instr;
      }
    }
  }
}

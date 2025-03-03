#include "test_pass.hpp"
#include <iostream>

using namespace std;

void TestPass::pass(Block &b) {
  for (auto &instr : b.instrs) {
    if (instr.op == IType::Br) {
      cout << "Br INSTRUCTION:" << endl;
      cout << instr;
    }
  }
}

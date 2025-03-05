#include "tdce.hpp"
#include <cassert>
#include <string>
#include <unordered_set>

using std::unordered_set, std::string;

void unused(Block &block) {
  unordered_set<string> declared_and_unused;
  for (auto it = block.instrs.begin(); it != block.instrs.end(); ++it) {
    switch (it->op) {
    case IType::Add:
    case IType::Eq:
    case IType::Sub: {
      auto &&[p1, p2] = it->vals.refp();
      declared_and_unused.erase(p1);
      declared_and_unused.erase(p2);
      assert(it->dest.has_value());
      auto &&dest = *it->dest;
      declared_and_unused.insert(dest);
    } break;
    case IType::Br: {
      auto &&h = it->vals.branches().cond();
      declared_and_unused.erase(h);
    } break;
    case IType::Print: {
      auto &&p1 = it->vals.args();
      for (auto &&arg : p1) {
        declared_and_unused.erase(arg);
      }
    } break;
    case IType::Ret: {
      auto &&p1 = it->vals.ref();
      declared_and_unused.erase(p1);
    } break;
    case IType::Const: {
      assert(it->dest.has_value());
      declared_and_unused.insert(*it->dest);
    } break;
    case IType::Call: {
      auto &&[_, args] = it->vals.fncall();
      for (auto &&arg : args) {
        declared_and_unused.erase(arg);
      }
      if (it->dest.has_value()) {
        declared_and_unused.insert(*it->dest);
      }
    } break;
    default:
      break;
    }
  }
  for (auto it = block.instrs.begin(); it != block.instrs.end(); ++it) {
    if (it->dest.has_value() && declared_and_unused.contains(*it->dest)) {
      it = block.instrs.erase(it);
    }
  }
}

void TDCEPass::pass(Block &b) { unused(b); }

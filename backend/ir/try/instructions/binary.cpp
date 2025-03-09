#include "binary.hpp"
#include "../value.hpp"
#include <cassert>
#include <memory>

using std::shared_ptr;

// Yeah there is a bit of code duplication here and there.
// I *probably* should fix it.
// Not exactly sure for now how though.

Add::Add(vptr dest, vptr val1, vptr val2) {
  assert(dest->type() == ValType::Ref);
  assert(val1->type() == ValType::Ref);
  assert(val2->type() == ValType::Ref);
  assert(std::dynamic_pointer_cast<Ref>(val1)->vtype() == Type::Int);
  assert(std::dynamic_pointer_cast<Ref>(val2)->vtype() == Type::Int);
  dest_ = dest;
  args.push_back(val1);
  args.push_back(val2);
}

Sub::Sub(vptr dest, vptr val1, vptr val2) {
  assert(dest->type() == ValType::Ref);
  assert(val1->type() == ValType::Ref);
  assert(val2->type() == ValType::Ref);
  assert(std::dynamic_pointer_cast<Ref>(val1)->vtype() == Type::Int);
  assert(std::dynamic_pointer_cast<Ref>(val2)->vtype() == Type::Int);
  dest_ = dest;
  args.push_back(val1);
  args.push_back(val2);
}

Br::Br(vptr cond, vptr l1, vptr l2) {
  assert(cond->type() == ValType::Ref);
  assert(std::dynamic_pointer_cast<Ref>(cond)->vtype() == Type::Bool);
  assert(l1->type() == ValType::Label);
  assert(l2->type() == ValType::Label);
  args.push_back(cond);
  args.push_back(l1);
  args.push_back(l2);
}

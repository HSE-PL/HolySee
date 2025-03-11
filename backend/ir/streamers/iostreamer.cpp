#include "iostreamer.hpp"
#include "../blocks/bblock.hpp"
#include "../fn.hpp"
#include "../instructions/binary.hpp"
#include "../instructions/instruction.hpp"
#include "../instructions/unary.hpp"
#include "../instructions/vararg.hpp"
#include "../program.hpp"
#include <iostream>
#include <iterator>
#include <unordered_map>

// nagovnokozheno krepko
// esli budut misli kak eto izmenit' to ispravlyu
// poka - good enough

std::unordered_map<Type, std::string> t2ss = {
    {Type::Int, "i32"},
    {Type::Bool, "bool"},
    {Type::Unit, "()"},
};

void IOStreamer::dest(Instruction &instr) {
  instr.dest_->accept(*this);
  stream << " = ";
}

void IOStreamer::args(Instruction &instr) {
  for (auto it = instr.begin(); it != instr.end(); ++it) {
    it->get()->accept(*this);
    if ((std::next(it)) != instr.end()) {
      stream << ", ";
    }
  }
}

void IOStreamer::visit(Add &add) {
  dest(add);
  stream << "add ";
  args(add);
}

void IOStreamer::visit(Sub &sub) {
  dest(sub);
  stream << "sub ";
  args(sub);
}

void IOStreamer::visit(Const &c) {
  dest(c);
  stream << "const ";
  args(c);
}

void IOStreamer::visit(Eq &eq) {
  dest(eq);
  stream << "eq ";
  args(eq);
}

void IOStreamer::visit(Print &print) {
  stream << "print ";
  args(print);
}

void IOStreamer::visit(Br &br) {
  stream << "br ";
  args(br);
}

void IOStreamer::visit(Call &call) {
  if (!(call.type() == ValType::Nothing)) {
    dest(call);
  }
  stream << "call ";
  args(call);
}

void IOStreamer::visit(Ret &ret) {
  stream << "ret ";
  args(ret);
}

void IOStreamer::visit(Jmp &jmp) {
  stream << "jmp ";
  args(jmp);
}

void IOStreamer::visit(Ref &ref) {
  stream << t2ss.at(ref.vtype()) << " " << ref.name();
}
void IOStreamer::visit(Label &label) {
  stream << "label" << " " << label.label();
}
void IOStreamer::visit(Int &integer) {
  stream << t2ss.at(Type::Int) << " " << integer.val();
}
void IOStreamer::visit(Bool &boolean) {
  stream << t2ss.at(Type::Bool) << " " << boolean.val();
}
void IOStreamer::visit(Unit &unit) { stream << t2ss.at(Type::Unit); }

void IOStreamer::visit(Program &program) {
  for (auto &&fn : program.funcs) {
    fn.second->accept(*this);
  }
}
void IOStreamer::visit(BBlock &b) {
  for (auto &&instr : b.instrs) {
    stream << "  ";
    instr->accept(*this);
    stream << std::endl;
  }
}
void IOStreamer::parameters(Fn &func) {
  for (auto it = func.args.begin(); it != func.args.end(); ++it) {
    it->get()->accept(*this);
    if ((std::next(it)) != func.args.end()) {
      stream << ", ";
    }
  }
}

void IOStreamer::visit(Fn &fn) {
  stream << t2ss.at(fn.type()) << " " << fn.name() << "(";
  parameters(fn);
  stream << ") {" << std::endl;
  for (auto &&block : fn.blocks) {
    block->accept(*this);
  }
  stream << "}" << std::endl;
}

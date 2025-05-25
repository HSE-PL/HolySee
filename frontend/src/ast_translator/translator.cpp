#include "translator.hpp"
#include "../ir/factory/ifactory.hpp"
#include "../lang/ast.hpp"
#include <iostream>

static size_t counter = 0;

static std::string ttToString(IR::TypeType tt) {
  switch (tt) {
  case IR::TypeType::Int:
    return "int";
    break;
  case IR::TypeType::Bool:
    return "bool";
    break;
  case IR::TypeType::Custom:
    return "custom";
    break;
  case IR::TypeType::Unit:
    return "_";
    break;
  }
}

static IR::TypeType type2type(AST::TypeClass tc) {
  switch (tc) {
  case AST::TypeClass::Int: {
    return IR::TypeType::Int;
  } break;
  case AST::TypeClass::Bool: {
    return IR::TypeType::Bool;
  } break;
  case AST::TypeClass::Void: {
    return IR::TypeType::Unit;
  } break;
  case AST::TypeClass::Custom: {
    return IR::TypeType::Custom;
  } break;
  }
}

static IR::Type irType(AST::TypeClass tc) {
  auto typetype = type2type(tc);
  return IR::Type{0, typetype};
}

static IR::Type irType(AST::TypeEntry entry) {
  if (entry.isPrimitive())
    return irType(entry.tclass);

  auto tt = type2type(entry.tclass);
  // here we should do lookup in typestuff.

  return IR::Type{0, tt};
}

std::string createTempName() { return "%" + std::to_string(counter++); }

std::shared_ptr<IR::Ref> ASTTranslator::createTemp(AST::TypeClass tc) {
  auto type = irType(tc);
  auto var = vfactory.createRef(type, createTempName());
  return var;
}

std::shared_ptr<IR::Value> ASTTranslator::visit(AST::Const &c) {
  auto val = c.value;
  auto rhs = vfactory.createInt(val);
  auto lhs = createTemp(AST::TypeClass::Int);
  auto instruction = ifactory.createConst(lhs, rhs);
  cblock.addInstr(instruction);
  return lhs;
}

std::shared_ptr<IR::Value> ASTTranslator::visit(AST::Var &var) {
  auto type = irType(var.type);
  return vfactory.createRef(type, var.id);
}

std::shared_ptr<IR::Value> ASTTranslator::visit(AST::Ret &ret) {
  std::shared_ptr<IR::Value> retExpr;
  if (!ret.retExpr.has_value()) {
    retExpr = vfactory.createUnit();
  } else {
    retExpr = (*ret.retExpr)->accept(*this);
  }
  auto reti = ifactory.createRet(retExpr);
  cblock.addInstr(reti);
  return retExpr;
}

std::shared_ptr<IR::Value> ASTTranslator::visit(AST::Assign &as) {
  auto rhs = as.expr->accept(*this);
  auto lhs = as.var->accept(*this);
  auto movi = ifactory.createMov(lhs, rhs);
  cblock.addInstr(movi);
  return lhs;
}

std::shared_ptr<IR::Value> ASTTranslator::visit(AST::Call &call) {
  auto fnOpt = ctx.tu->fnLookup(call.fn);
  if (!fnOpt.has_value()) {
    throw "call of undefined function " + call.fn;
  }
  auto fn = *fnOpt;
  std::list<std::shared_ptr<IR::Value>> args;
  for (auto &&arg : call.args) {
    args.push_back(arg->accept(*this));
  }
  auto dest = createTemp(fn->type.tclass);
  auto calli = ifactory.createCall(call.fn, dest, args);
  cblock.addInstr(calli);

  return dest;
}

std::shared_ptr<IR::Value> ASTTranslator::visit(AST::Stmt &stmt) {
  return stmt.accept(*this);
}

std::shared_ptr<IR::Value> ASTTranslator::visit(AST::Expr &expr) {
  return expr.accept(*this);
}

std::shared_ptr<IR::Value> ASTTranslator::visit(AST::BinExp &bin) {
  auto rhs = bin.rhs->accept(*this);
  auto lhs = bin.lhs->accept(*this);
  std::shared_ptr<IR::Instruction> instr;
  std::shared_ptr<IR::Ref> dest;
  switch (bin.op) {
  case AST::BinOp::Add: {
    auto t = AST::TypeClass::Int;
    dest = createTemp(t);
    instr = ifactory.createAdd(dest, lhs, rhs);
  } break;
  case AST::BinOp::Sub: {
    auto t = AST::TypeClass::Int;
    dest = createTemp(t);
    instr = ifactory.createSub(dest, lhs, rhs);
  } break;
  case AST::BinOp::Mul: {
    auto t = AST::TypeClass::Int;
    dest = createTemp(t);
    instr = ifactory.createMul(dest, lhs, rhs);
  } break;
  case AST::BinOp::Div: {
    auto t = AST::TypeClass::Int;
    dest = createTemp(t);
    instr = ifactory.createDiv(dest, lhs, rhs);
  } break;
  case AST::BinOp::And: {
    auto t = AST::TypeClass::Bool;
    dest = createTemp(t);
    instr = ifactory.createAnd(dest, lhs, rhs);
  } break;
  case AST::BinOp::Or: {
    auto t = AST::TypeClass::Bool;
    dest = createTemp(t);
    instr = ifactory.createOr(dest, lhs, rhs);
  } break;
  }
  cblock.addInstr(instr);
  return dest;
}

std::shared_ptr<IR::Fn> ASTTranslator::visitFn(AST::Function &fn) {
  auto fnName = fn.id;
  auto type = irType(fn.type);
  cfn.reset(type, fnName);
  cblock.reset(fnName);

  for (auto &&par : fn.params) {
    auto type = irType(par->type);
    cfn.addArg(type, par->id);
  }

  for (auto &&stmt : fn.body) {
    stmt->accept(*this);
  }

  auto fblock = cblock.build();
  cfn.addBlock(fblock);
  auto function = cfn.build();
  return function;
}

IR::Program ASTTranslator::translate(AST::TranslationUnit &unit) {
  IR::Program program;
  ctx.tu = &unit;
  for (auto &&[_, fn] : unit.funs) {
    auto function = visitFn(*fn);
    program.addFunc(function);
  }
  return program;
}

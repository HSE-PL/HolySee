#include "translator.hpp"
#include "../ir/factory/ifactory.hpp"
#include "../lang/ast.hpp"
#include <iostream>
#include <memory>
#include <string>

static size_t counter = 0;
static size_t tcounter = 0;
static size_t fcounter = 0;
static size_t ecounter = 0;
static size_t endcounter = 0;
static size_t wccounter = 0;
static size_t wendcounter = 0;
static size_t wbcounter = 0;

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

void ASTTranslator::endBlock(std::string new_block) {
  auto block = cblock.build();
  cfn.addBlock(block);
  cblock.reset(new_block);
}

static IR::Type irType(AST::TypeClass tc) {
  auto typetype = type2type(tc);
  return IR::Type{ttToString(type2type(tc)), typetype};
}

static IR::Type irType(AST::TypeEntry entry) {
  if (entry.isPrimitive())
    return irType(entry.tclass);

  auto tt = type2type(entry.tclass);
  // here we should do lookup in typestuff.

  return IR::Type{entry.name, tt};
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
  if (!fnOpt.has_value() && call.fn != "println") {
    throw "call of undefined function " + call.fn;
  }
  std::list<std::shared_ptr<IR::Value>> args;
  for (auto &&arg : call.args) {
    args.push_back(arg->accept(*this));
  }
  if (call.fn == "println") {
    auto printing = ifactory.createPrint(args);
    cblock.addInstr(printing);
    return printing;
  }
  auto fn = *fnOpt;
  auto dest = createTemp(fn->type.tclass);
  auto calli = ifactory.createCall(call.fn, dest, args);
  cblock.addInstr(calli);

  return dest;
}

std::shared_ptr<IR::CompositeType>
ASTTranslator::visitTDecl(AST::TypeDeclaration &td) {
  auto typeName = td.type;
  std::vector<IR::Type> types;
  std::vector<std::string> fieldnames;
  for (auto &&field : td.fields) {
    fieldnames.push_back(field->id);
    auto type = irType(field->type);
    types.push_back(type);
  }
  auto ret = std::make_shared<IR::CompositeType>(typeName, types, fieldnames);

  return ret;
}

std::shared_ptr<IR::Value> ASTTranslator::visit(AST::VarDecl &vd) {
  for (auto &&var : vd.vars) {
    //
    if (var->type.isPrimitive()) {
      // TODO: here we just need to zero it out.
      continue;
    }
    // now the real war begins.
    auto type = irType(var->type);
    auto dest = vfactory.createRef(type, var->id);
    auto tptr = std::make_shared<IR::Type>(type);
    auto alloc = ifactory.createAlloc(dest, tptr);
    cblock.addInstr(alloc);
  }
  return vfactory.createUnit();
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
  case AST::BinOp::Less: {
    auto t = AST::TypeClass::Bool;
    dest = createTemp(t);
    instr = ifactory.createCmp(dest, lhs, rhs, IR::Predicate::LT);
  } break;
  case AST::BinOp::LessEqual: {
    auto t = AST::TypeClass::Bool;
    dest = createTemp(t);
    instr = ifactory.createCmp(dest, lhs, rhs, IR::Predicate::LE);
  } break;
  case AST::BinOp::Greater: {
    auto t = AST::TypeClass::Bool;
    dest = createTemp(t);
    instr = ifactory.createCmp(dest, lhs, rhs, IR::Predicate::GT);
  } break;
  case AST::BinOp::GreaterEqual: {
    auto t = AST::TypeClass::Bool;
    dest = createTemp(t);
    instr = ifactory.createCmp(dest, lhs, rhs, IR::Predicate::GE);
  } break;
  case AST::BinOp::Equals: {
    auto t = AST::TypeClass::Bool;
    dest = createTemp(t);
    instr = ifactory.createCmp(dest, lhs, rhs, IR::Predicate::EQ);
  } break;
  case AST::BinOp::NotEqual: {
    auto t = AST::TypeClass::Bool;
    dest = createTemp(t);
    instr = ifactory.createCmp(dest, lhs, rhs, IR::Predicate::NEQ);
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

std::shared_ptr<IR::Value> ASTTranslator::visit(AST::If &iff, std::string end) {
  auto cond = iff.cond->accept(*this);
  auto trueName = "if.true" + std::to_string(tcounter++);
  std::string falseName;
  bool recend = (!iff.elseBody.has_value()) && (!iff.elseif.has_value());
  bool haselse = iff.elseBody.has_value();
  bool haselseif = iff.elseif.has_value();

  if (recend) {
    falseName = end;
  } else if (haselse) {
    falseName = "if.false" + std::to_string(fcounter++);
  } else if (haselseif) {
    falseName = "if.else" + std::to_string(ecounter++);
  }

  auto ltrue = vfactory.createLabel(trueName);
  auto lfalse = vfactory.createLabel(falseName);
  auto br = ifactory.createBr(cond, ltrue, lfalse);
  cblock.addInstr(br);
  endBlock(trueName);

  for (auto &&stmt : iff.tbranch) {
    stmt->accept(*this);
  }

  if (recend) {
    auto jmptrue = ifactory.createJmp(lfalse);
    cblock.addInstr(jmptrue);
    endBlock(falseName);
    return lfalse;
  }

  if (haselse) {
    auto endName = end;
    auto lend = vfactory.createLabel(endName);
    auto jmptrue = ifactory.createJmp(lend);
    cblock.addInstr(jmptrue);
    endBlock(falseName);
    auto elsebody = *iff.elseBody;
    for (auto &&stmt : elsebody) {
      stmt->accept(*this);
    }
    jmptrue = ifactory.createJmp(lend);
    cblock.addInstr(jmptrue);
    endBlock(endName);
    return lend;
  }

  if (haselseif) {
    auto elseif = *iff.elseif;
    auto endName = end;
    auto lend = vfactory.createLabel(endName);
    auto jmptrue = ifactory.createJmp(lend);
    cblock.addInstr(jmptrue);
    endBlock(falseName);
    visit(*elseif, end);
    return lend;
  }
  throw "i didn't think we could get there tbh";
}

std::shared_ptr<IR::Value> ASTTranslator::visit(AST::If &iff) {
  return visit(iff, "if.end" + std::to_string(endcounter++));
}

std::shared_ptr<IR::Value> ASTTranslator::visit(AST::While &wh) {

  auto end = "while.end" + std::to_string(wendcounter++);
  auto cond = "while.cond" + std::to_string(wccounter++);
  auto body = "while.body" + std::to_string(wbcounter++);

  auto condb = vfactory.createLabel(cond);
  auto bodyb = vfactory.createLabel(body);
  auto endb = vfactory.createLabel(end);

  auto jmpToLoop = ifactory.createJmp(condb);
  cblock.addInstr(jmpToLoop);
  endBlock(cond);

  auto cd = wh.cond->accept(*this);
  auto br = ifactory.createBr(cd, bodyb, endb);
  cblock.addInstr(br);
  endBlock(body);

  for (auto &&stmt : wh.body) {
    stmt->accept(*this);
  }

  auto jmpToStart = ifactory.createJmp(condb);
  cblock.addInstr(jmpToStart);
  endBlock(end);

  return endb;
}

IR::Program ASTTranslator::translate(AST::TranslationUnit &unit) {
  IR::Program program;
  ctx.tu = &unit;
  for (auto &&[name, type] : unit.typeDecls) {
    auto irtype = visitTDecl(*type);
    program.addType(name, irtype);
  }
  for (auto &&[_, fn] : unit.funs) {
    auto function = visitFn(*fn);
    program.addFunc(function);
  }
  return program;
}

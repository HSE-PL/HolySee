#include "ast.hpp"
#include <unordered_map>

using namespace AST;

std::unordered_map<Predicate, std::string> predicateString = {
    {Predicate::EQ, "=="}, {Predicate::NEQ, "!="}, {Predicate::GE, ">="},
    {Predicate::LE, "<="}, {Predicate::GT, ">"},   {Predicate::LT, "<"},
};

std::unordered_map<BinOp, std::string> binOps = {
    {BinOp::Add, "+"},           {BinOp::Sub, "-"},
    {BinOp::Mul, "*"},           {BinOp::Mul, "/"},
    {BinOp::Less, "<"},          {BinOp::Greater, ">"},
    {BinOp::GreaterEqual, ">="}, {BinOp::LessEqual, "<="},
    {BinOp::Equals, "=="},       {BinOp::NotEqual, "!="},
    {BinOp::And, "and"},         {BinOp::Or, "or"},
};

std::string Cond::toString() {
  return "(" + lhs->toString() + " " + predicateString[p] + " " +
         rhs->toString() + ")";
}

std::string BinExp::toString() {
  return "(" + lhs->toString() + " " + binOps[op] + " " + rhs->toString() + ")";
}

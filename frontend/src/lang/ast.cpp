#include "ast.hpp"
#include <unordered_map>

std::unordered_map<Predicate, std::string> predicateString = {
    {Predicate::EQ, "=="}, {Predicate::NEQ, "!="}, {Predicate::GE, ">="},
    {Predicate::LE, "<="}, {Predicate::GT, ">"},   {Predicate::LT, "<"},
};

std::string Cond::toString() {
  return "(" + lhs->toString() + " " + predicateString[p] + " " +
         rhs->toString() + ")";
}

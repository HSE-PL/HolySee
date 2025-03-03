#pragma once

#include <string>
#include <unordered_map>

enum class IType {
  Add,
  Sub,
  Const,
  Print,
  Ret,
  Call,
  Jmp,
  Br,
  Eq,
};

IType string2instr(std::string instr);

extern std::unordered_map<IType, std::string> i2s;
extern std::unordered_map<std::string, IType> s2i;

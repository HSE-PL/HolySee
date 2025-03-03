#include "itype.hpp"
#include "../../backend.hpp"

std::unordered_map<IType, std::string> i2s = {
    {IType::Const, "const"}, {IType::Add, "add"},   {IType::Print, "print"},
    {IType::Ret, "ret"},     {IType::Call, "call"}, {IType::Br, "br"},
    {IType::Jmp, "j"},       {IType::Eq, "eq"},     {IType::Sub, "sub"},
};

std::unordered_map<std::string, IType> s2i = {
    {"const", IType::Const}, {"add", IType::Add},   {"print", IType::Print},
    {"ret", IType::Ret},     {"call", IType::Call}, {"br", IType::Br},
    {"jmp", IType::Jmp},     {"eq", IType::Eq},     {"sub", IType::Sub},
};

IType string2instr(std::string instr) {
  if (!s2i.contains(instr)) {
    throw NotImplemented("such instruction doesn't exist" + instr);
  }
  return s2i.at(instr);
}

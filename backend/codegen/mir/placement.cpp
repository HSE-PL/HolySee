#include "placement.hpp"
#include <string>
#include <unordered_map>

const std::unordered_map<size_t, std::string> regs2str = {
    {(size_t)GPR::rax, "rax"}, {(size_t)GPR::rbx, "rbx"},
    {(size_t)GPR::rcx, "rcx"}, {(size_t)GPR::rdx, "rdx"},
    {(size_t)GPR::rdi, "rdi"}, {(size_t)GPR::rsi, "rsi"},
    {(size_t)GPR::r8, "r8"},   {(size_t)GPR::r9, "r9"},
    {(size_t)GPR::r10, "r10"}, {(size_t)GPR::r11, "r11"},
    {(size_t)GPR::r12, "r12"}, {(size_t)GPR::r13, "r13"},
    {(size_t)GPR::r14, "r14"}, {(size_t)GPR::r15, "r15"},
    {(size_t)GPR::rbp, "rbp"}, {(size_t)GPR::rsp, "rsp"},
};

std::string Placement::emit() {
  switch (tp_) {
  case Register:
    return regs2str.at(place());
    break;
  case Stack:
    auto actualPlace = place() * 8;
    return "qword [rbp - " + std::to_string(actualPlace) +
           "]"; // HARDCODED AS IT SHOULD BE!.
    break;
  }
}

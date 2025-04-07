#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>
enum PlacementType {
  Register,
  Stack
}; // instead of stack it should be memory. fine for now,
   // since i don't really generate any static sheisung and we don't have heap
   // either(in practice).

enum class GPR {
  rax,
  rbp,
  rsp,
  rbx,
  rdx,
  rcx,
  rdi,
  rsi,
  r8,
  r9,
  r10,
  r11,
  r12,
  r13,
  r14,
  r15,
};

class Placement {
  PlacementType tp_;
  size_t place_; // either place on current stack frame or register number.
public:
  size_t place() const { return place_; }
  std::string emit();
  PlacementType type() const { return tp_; }
  Placement(PlacementType tp, size_t place) : tp_(tp), place_(place) {}
};

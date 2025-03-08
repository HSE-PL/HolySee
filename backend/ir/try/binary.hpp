#pragma once

#include "instruction.hpp"

class Add final : public Instruction {
  friend class IFactory;
  Add(vptr dest, vptr val1, vptr val2);

public:
};

class Sub final : public Instruction {
  friend class IFactory;
  Sub(vptr dest, vptr val1, vptr val2);

public:
};

class Br final : public Instruction {
  friend class IFactory;
  Br(vptr cond, vptr l1, vptr l2);
};

#include "Safepoint.hpp"
#include "system/System.hpp"
#include "utils/defines.h"
#include "utils/log.h"
#include <iostream>
#include <locale>
#include <sys/mman.h>
void sp::change(int prot) {
  mprotect(spd, 1_page, prot);
}

void sp::off() {
  sp = true;
  change(PROT_NONE);
}

void sp::on() {
  sp = false;
  change(PROT_READ);
}

void sp::init(void** spdptr) {
  spd = mmap(nullptr, 1_page, PROT_READ, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
  if (spd == MAP_FAILED)
    throw std::runtime_error("salloc return shit in sp::init\n");
  *spdptr = spd;
}
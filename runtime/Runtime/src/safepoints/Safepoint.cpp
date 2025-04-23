#include "Safepoint.hpp"
#include "system/System.hpp"

#include "utils/log.h"
#include <iostream>
#include <locale>
#include <sys/mman.h>
void sp::change(int prot) {
  mprotect(spd, pagesize, prot);
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
  spd = sys::salloc(pagesize);
  if (!spd)
    throw std::runtime_error("salloc return shit in sp::init\n");
  log << "spd on: " << spd << "\n";
  *spdptr = spd;
}
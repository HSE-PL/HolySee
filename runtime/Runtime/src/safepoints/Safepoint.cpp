#include "Safepoint.hpp"
#include "system/System.hpp"

#include <iostream>
#include <locale>
#include <sys/mman.h>

void sp::change(int prot) {
  mprotect(spd, pagesize, prot);
}

void sp::off() {
  change(PROT_NONE);
}

void sp::on() {
  change(PROT_READ);
}

void sp::init(void** spdptr) {
  spd = sys::salloc(pagesize);
  if (!spd)
    throw std::runtime_error(
        "salloc return shit in sp::init\n");
  *spdptr = spd;
}
#pragma once

#define guard(mutex) std::lock_guard _(mutex)
typedef unsigned long long ref;
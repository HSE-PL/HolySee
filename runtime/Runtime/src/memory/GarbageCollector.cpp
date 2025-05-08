#include "GarbageCollector.hpp"
#include "Runtime.hpp"
#include "safepoints/Safepoint.hpp"
#include "threads/Threads.hpp"
#include "utils/log.h"
#include <cassert>
#include <cstddef>
#include <iomanip>

GarbageCollector::GarbageCollector(instance* first, instance* last)
    : inteval_(first, last) {
}

auto GarbageCollector::how_many_ref(instance* p) -> size_t {
  return p->have_ref ? p->size : 0;
}

auto GarbageCollector::how_many_ref(ref p) -> size_t {
  auto i = *reinterpret_cast<instance**>(p - 1_ref);
  if (inteval_.correct(i))
    return how_many_ref(i);
  return 0;
}

auto GarbageCollector::cleaning() -> void {
  auto trash = std::vector<Arena*>();

  for (const auto& arena : Allocator::instance().heap.keys) {
    if (arena->is_died())
      trash.push_back(arena);
  }

  for (const auto& trash_arena : trash)
    Allocator::instance().free_arena(trash_arena);

  for (const auto& a : Allocator::instance().heap.keys)
    a->temp_kill();

  ++num_of_cleaning;
}

auto GarbageCollector::marking(ref ptr) -> void {
  if (ref_is_real(ptr)) {
    auto i = (*reinterpret_cast<instance**>(ptr - 1_ref));
    logezhe << "marking: try to marking " << ptr << " type (" << i->name
            << ")\n";
  }
  auto arena = MemoryManager::arena_by_ptr(ptr);
  if (arena->marked_.check_and_set(ptr))
    return;

  arena->revive();
  auto size = how_many_ref(ptr);
  for (auto offset = 0; offset < size; offset += 1_ref) {
    if (ref_is_real(ptr + offset)) {
      if (auto field = *reinterpret_cast<ref*>(ptr); ref_is_real(field)) {
        logezhe << "marking_push: " << ptr << "\n";
        stack_for_marked_.push(field);
      }
    }
  }
  logezhe << "marking object: end\n";
}

auto GarbageCollector::ref_is_real(ref ptr) -> bool {
  return MemoryManager::ref_in_heap(ptr) && how_many_ref(ptr);
}

auto GarbageCollector::GC() -> void {
  auto s = clock();
  for (auto i = 0; i < threads::Threads::instance().count(); ++i)
    threads::Threads::instance().tracing_.acquire();

  std::atomic_thread_fence(std::memory_order_acquire);
  cleaning();
  std::atomic_thread_fence(std::memory_order_release);

  sp::on();
  threads::Threads::instance().cleaning_.release(
      static_cast<std::ptrdiff_t>(threads::Threads::instance().count()));

  std::cout << "STW end, time: " << std::fixed << std::setprecision(3)
            << 1000 * static_cast<double>(clock() - s) / CLOCKS_PER_SEC
            << "ms \n";
}

auto GarbageCollector::tracing() -> void {
  while (true) {
    if (auto ptr = stack_for_marked_.pop().value_or(0); ptr)
      marking(ptr);
    else {
      if (!threads::Threads::instance().count_of_working_threads_)
        throw std::runtime_error("fantom working thread");

      if (!--threads::Threads::instance().count_of_working_threads_) {
        stack_for_marked_.clear();
        break;
      }

      while (stack_for_marked_.empty())
        if (!threads::Threads::instance().count_of_working_threads_)
          return;
      ++threads::Threads::instance().count_of_working_threads_;
    }
  }
}

auto GarbageCollector::make_root_and_tracing(ref ssp) -> void {
  auto hrtptr = threads::Threads::instance().get(ssp);
  logezhe << hrtptr.start_sp << ":\n.\n.\n.\n"
          << ssp << "\n"
          << "summary diff: " /*<< std::hex*/
          << hrtptr.start_sp - ssp << "\n";

  assert(ssp < hrtptr.start_sp);
  for (ref sp = ssp; sp <= hrtptr.start_sp; sp += 1_ref) {
    auto ptr = *reinterpret_cast<ref*>(sp);
    logezhe << "make_root_and_tracing: on stack: " << ptr << "\n";
    if (ref_is_real(ptr)) {
      logezhe << "find " << ptr << " in root\n";
      stack_for_marked_.push(ptr);
    }
  }
  tracing();

  std::atomic_thread_fence(std::memory_order_release);
  threads::Threads::instance()
      .tracing_.release(); // tell gc we`re done! (for start cleaning)
}

auto GarbageCollector::gogc(ref ssp) -> void {
  auto n = num_of_cleaning.load();
  std::atomic_thread_fence(std::memory_order_release);
  ++threads::Threads::instance().count_of_working_threads_;
  threads::Threads::instance().wait_end_sp();

  make_root_and_tracing(ssp);
  for (;;) {
    if (auto new_n = num_of_cleaning.load(); n + 1 == new_n) {
      threads::Threads::instance()
          .cleaning_.acquire(); // waiting for the end of cleaning
      break;
    }
  }
  std::atomic_thread_fence(std::memory_order_acquire);
}
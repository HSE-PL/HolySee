#include "Allocator.hpp"
#include "utils/log.h"

#include <sstream>

fn partion_of_pages(size_t count_pages)->std::vector<size_t> {
  int count;
  for (count = 1; count * (1 << count) <= count_pages; ++count)
    ;

  --count;

  std::vector<size_t> counts;

  for (int i = count; i; counts.push_back(1 << i--))
    ;

  size_t lost_pages = count_pages - count * (1 << count);

  let lost_partion =
      lost_pages < 8 ? std::vector<size_t>(lost_pages, 1) : partion_of_pages(lost_pages);

  for (int i = 0; i < lost_partion.size(); ++i)
    counts[i] += lost_partion[i];

  return counts;
}

Allocator::Allocator(ref start, size_t size)
    // , emplaced_(start, start + size)
    : start_(start), size_(size), marking_(start, start + size),
      regions_(std::vector<Region<Arena>*>()) {
  log << "heap on\n" << start << "\n" << start + size << "\n size: " << size;
  /*
   * variant of regions "double-double"
   * each egion is +- equal in size
   */
  size_t count_pages = size >> 12;

  auto counts = partion_of_pages(count_pages);

  auto   cur    = start;
  size_t a_size = 1 << 12;
  size_t i      = 0;
  log << "\n-----partion:\n";
  for (const size_t c : counts) {
    log << c << " \n";
    regions_.push_back(new Region<Arena>{cur, c, a_size, i});

    add_active(i++);

    cur += c * a_size;
    a_size <<= 1;
  }
  // log << "\n";
  // heap_.print();
  dump();
}

fn Allocator::alloc(size_t object_size)->ref {
  guard(mutex_);
  let arena = heap_.get_min_more_then(object_size);
  log << arena << "\n";
  if (arena == nullptr)
    return 0;
  log << "alloca " << object_size;
  log << " on arena " << arena->start << "\n";
  ref start_new_object = arena->cur;
  // log << start_new_object << "\n";
  // log << "alloc: current pull: " << regions_[arena->tier]->pull_.v_.size() << "\n";
  if (start_new_object == arena->start && (regions_[arena->tier]->size_pull() || false))
    add_active(arena->tier);
  // dump();
  // log << "try to del\n";
  heap_.del(arena);
  // dump();
  log << "arena was del\n";
  arena->cur += object_size;
  log << "huh\n";
  // arena->objects.push(object_size);
  log << "huh\n";
  heap_.append(arena);
  // dump();
  log << "arena was replace\n";

  // emplaced_.set(start_new_object);
  // log << "alloc: current pull: " << regions_[arena->tier]->pull_.v_.size() << "\n";
  dump();
  log << "alloc: end\n";
  return start_new_object;
}

fn Allocator::add_active(size_t index)->void {
  guard(mutex_);
  log << "call add active\n";
  let new_active = regions_[index]->pop();

  log << "call append new_active\n" << new_active->uniq_for_heap() << "\n";
  heap_.append(new_active);
  log << "return from add_active\n";
}

fn Allocator::arena_by_ptr(ref ptr) const->Arena* {

  int index;
  for (index = 0; index < regions_.size() && ptr >= regions_[index]->start; ++index)
    ;
  --index;
  let offset  = ptr - regions_[index]->start;
  let a_index = offset / regions_[index]->t_size;
  // log << "arena_by_ptr: complete\n";
  return regions_[index]->operator[](a_index);
}

fn Allocator::free_arena(Arena* a)->void {
  guard(mutex_);
  log << a->uniq_for_heap() << "\n";
  log << "info:\n";
  log << "tier: " << a->tier;
  log << "\nfree: " << a->key_for_heap() << "\n";
  a->kill();
  log << "?\n";
  heap_.del(a);
  a->cur = a->start;
  regions_[a->tier]->push(a);
}

fn Allocator::revive(Arena* a)->void {
  free_arena(a);
  add_active(a->tier);
  // heap_.append(a);
  log << "revive: end\n";
}

// 4 debug and test only
fn Allocator::free(ref ptr)->void {
  guard(mutex_);
  log << "free     " << ptr << "\n";
  log << "on arena " << arena_by_ptr(ptr)->start << "\n";
  let abp = arena_by_ptr(ptr);
  // emplaced_.clear(abp->start, abp->start + abp->size);
  free_arena(abp);
}

fn Allocator::dump() const->void {
  return;
  // std::ostringstream log;
  log << std::hex;

  for (const auto& r : regions_) {
    log << r->t_size << " region : {";
    for (const auto& arena : heap_.keys) {
      if (arena->size == r->t_size) {
        log << "arena " << arena->start << " (";
        // guard(arena->mutex_);
        for (const auto& obj_size : arena->objects) {
          log << "[" << obj_size << "],";
        }
        log << "... " << arena->key_for_heap() << "), <"
            << regions_[arena->tier]->size_pull() << "> ";
      }
    }
    log << "}\n";
  }

  // log << oss.str();
}


fn BitMap::set(ref n)->void {
  guard(mutex_);
  Bitset::set(map(n));
}

// void BitMap::clear() {
//   guard(mutex_);
//   Bitset::clear();
// }

fn BitMap::clear(ref from, ref to)->void {
  guard(mutex_);
  Bitset::clear(map(from), map(to));
}

fn BitMap::check_and_set(ref n)->bool {
  guard(mutex_);
  let mask = get(map(n));
  set(n);
  return mask;
}
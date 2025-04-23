#include "Allocator.hpp"
#include "utils/log.h"

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
    : start_(start), size_(size), emplaced_(start, start + size), marking_(start, start + size),
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
  log << "-----partion:\n";
  for (const size_t c : counts) {
    log << c << " \n";
    regions_.push_back(new Region<Arena>{cur, c, a_size, i});

    add_active(i++);

    cur += c * a_size;
    a_size <<= 1;
  }
  log << "\n";
  heap_.print();
}

fn Allocator::alloc(size_t object_size)->ref {
  guard(mutex_);
  auto arena = heap_.get_min_more_then(object_size);
  log << "alloca " << object_size << " on arena " << arena->start << "\n";
  ref start_new_object = arena->cur;
  log << start_new_object << "\n";
  if (start_new_object == arena->start && regions_[arena->tier]->size_pull())
    add_active(arena->tier);

  log << "try to del\n";
  heap_.del(arena);
  log << "arena was del\n";
  arena->cur += object_size;
  heap_.append(arena);
  log << "arena was replace\n";
  emplaced_.set(start_new_object);
  heap_.print();
  return start_new_object;
}

fn Allocator::add_active(size_t index)->void {
  guard(mutex_);
  log << "call add active\n";
  let new_active = regions_[index]->pop();

  // regions[index].count_empty++;

  log << "call append new_active\n" << new_active << "\n";
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
  log << "arena_by_ptr: complete\n";
  return regions_[index]->operator[](a_index);
}

fn Allocator::free_arena(Arena* a)->void {
  guard(mutex_);
  heap_.del(a);
  a->cur = a->start;
  regions_[a->tier]->push(a);
}

fn Allocator::revive(Arena* a)->void {
  free_arena(a);
  heap_.append(a);
}

// for debug and test only
fn Allocator::free(ref ptr)->void {
  guard(mutex_);
  log << "free     " << ptr << "\n";
  log << "on arena " << arena_by_ptr(ptr)->start << "\n";
  let abp = arena_by_ptr(ptr);
  emplaced_.clear(abp->start, abp->start + abp->size);
  free_arena(abp);
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
  set(n);
  return get(map(n));
}
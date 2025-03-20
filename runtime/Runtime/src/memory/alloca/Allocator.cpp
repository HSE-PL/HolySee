#include "Allocator.hpp"
#include "utils/log.h"

std::vector<size_t> partion_of_pages(size_t count_pages) {
  int count;
  for (count = 1; count * (1 << count) <= count_pages; ++count)
    ;

  --count;

  std::vector<size_t> counts;

  for (int i = count; i; counts.push_back(1 << i--))
    ;

  size_t lost_pages = count_pages - count * (1 << count);

  auto lost_partion =
      lost_pages < 8 ? std::vector<size_t>(lost_pages, 1) : partion_of_pages(lost_pages);

  for (int i = 0; i < lost_partion.size(); ++i)
    counts[i] += lost_partion[i];

  return counts;
}

Allocator::Allocator(ref start, size_t size)
    : Heap(), start_(start), size_(size), emplaced_(start, start + size),
      marking_(start, start + size), regions_(std::vector<Region<Arena>*>()) {
  log << "heap on\n" << start << "\n" << start + size << "\n";
  /*
   * variant of regions "double-double"
   * each egion is +- equal in size
   */
  size_t count_pages = size >> 12;

  auto counts = partion_of_pages(count_pages);

  auto   cur    = start;
  size_t a_size = 1 << 12;
  size_t i      = 0;
  for (const size_t c : counts) {
    regions_.push_back(new Region<Arena>{cur, c, a_size, i});

    add_active(i++);

    cur += c * a_size;
    a_size <<= 1;
  }
  print();
}

ref Allocator::alloc(size_t object_size) {
  guard(mutex_);
  auto arena = get_min_more_then(object_size);
  log << "alloca " << object_size << " on arena " << arena->start << "\n";
  ref start_new_object = arena->cur;
  log << start_new_object << "\n";
  if (start_new_object == arena->start && regions_[arena->tier]->size_pull())
    add_active(arena->tier);

  log << "try to del\n";
  del(arena);
  log << "arena was del\n";
  arena->cur += object_size;
  append(arena);
  log << "arena was replace\n";
  emplaced_.set(start_new_object);
  print();
  return start_new_object;
}

void Allocator::add_active(size_t index) {
  guard(mutex_);
  log << "call add active\n";
  auto new_active = regions_[index]->back();
  regions_[index]->pop();

  // regions[index].count_empty++;

  log << "call append new_active\n" << new_active << "\n";
  append(new_active);
  log << "return from add_active\n";
}

Arena* Allocator::arena_by_ptr(ref ptr) const {
  int index;
  for (index = 0; index < regions_.size() && ptr >= regions_[index]->start; ++index)
    ;
  --index;
  auto offset  = ptr - regions_[index]->start;
  auto a_index = offset / regions_[index]->t_size;
  return regions_[index]->operator[](a_index);
}

void Allocator::free_arena(Arena* a) {
  guard(mutex_);
  del(a);
  a->cur = a->start;
  regions_[a->tier]->push(a);
}

void Allocator::revive(Arena* a) {
  free_arena(a);
  append(a);
}

// for debug and test only
void Allocator::free(ref ptr) {
  guard(mutex_);
  log << "free     " << ptr << "\n";
  log << "on arena " << arena_by_ptr(ptr)->start << "\n";
  auto abp = arena_by_ptr(ptr);
  emplaced_.clear(abp->start, abp->start + abp->size);
  free_arena(abp);
}

void BitMap::set(ref n) {
  guard(mutex_);
  Bitset::set(map(n));
}

bool BitMap::operator[](ref n) {
  guard(mutex_);
  return get(map(n));
}

void BitMap::clear() {
  guard(mutex_);
  Bitset::clear();
}

void BitMap::clear(ref from, ref to) {
  guard(mutex_);
  Bitset::clear(map(from), map(to));
}
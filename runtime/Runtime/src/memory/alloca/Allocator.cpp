#include "Allocator.hpp"
#include "utils/log.h"

std::vector<size_t>
partion_of_pages(size_t count_pages) {
  int count;
  for (count = 1;
       count * (1 << count) <= count_pages;
       ++count)
    ;

  --count;

  std::vector<size_t> counts;

  for (int i = count; i;
       counts.push_back(1 << i--))
    ;

  size_t lost_pages =
      count_pages - count * (1 << count);

  auto lost_partion =
      lost_pages < 8
          ? std::vector<size_t>(lost_pages, 1)
          : partion_of_pages(lost_pages);

  for (int i = 0; i < lost_partion.size(); ++i)
    counts[i] += lost_partion[i];

  return counts;
}

Allocator::Allocator(size_t start, size_t size)
    : Heap(), start_(start), size_(size),
      emplaced_(start, start + size),
      regions_(std::vector<Region<Arena>>()) {
  log << "heap on\n"
      << start << "\n"
      << start + size << "\n";
  /*
   * variant of regions "double-double"
   * each egion is +- equal in size
   */
  size_t count_pages = size >> 12;

  auto counts = partion_of_pages(count_pages);

  size_t cur    = start;
  size_t a_size = 1 << 12;
  size_t i      = 0;
  for (const size_t c : counts) {
    regions_.emplace_back(cur, c, a_size, i);

    add_active(i++);

    cur += c * a_size;
    a_size <<= 1;
  }
}

size_t Allocator::alloc(size_t object_size) {
  mutex_.lock();
  auto arena = get_min_more_then(object_size);
  log << "alloca " << object_size << " on arena "
      << arena << "\n";
  size_t start_new_object = arena->cur;
  log << start_new_object << "\n";
  if (start_new_object == arena->start)
    add_active(arena->tier);

  log << "try to del\n";
  del(arena);
  log << "arena was del\n";
  arena->cur += object_size;
  append(arena);
  log << "arena was replace\n";
  emplaced_.set(start_new_object);
  mutex_.unlock();
  return start_new_object;
}

void Allocator::add_active(size_t index) {
  mutex_.lock();
  log << "call add active\n";
  auto new_active = regions_[index].pull.back();
  regions_[index].pull.pop_back();

  // regions[index].count_empty++;

  log << "call append new_active\n"
      << new_active << "\n";
  append(new_active);
  log << "return from add_active\n";
  mutex_.unlock();
}

Arena* Allocator::arena_by_ptr(size_t ptr) const {
  int index;
  for (index = 0; index < regions_.size() &&
                  ptr >= regions_[index].start;
       ++index)
    ;
  --index;
  auto offset  = ptr - regions_[index].start;
  auto a_index = offset / regions_[index].t_size;
  return regions_[index].items[a_index];
}

void Allocator::free_arena(Arena* a) {
  mutex_.lock();
  del(a);
  keys.erase(a);
  a->cur = a->start;
  regions_[a->tier].pull.push_back(a);
  mutex_.unlock();
}

void Allocator::free(size_t ptr) {
  mutex_.lock();
  log << "free     " << ptr << "\n";
  log << "on arena " << arena_by_ptr(ptr)->start
      << "\n";
  auto abp = arena_by_ptr(ptr);
  emplaced_.clear(abp->start,
                  abp->start + abp->size);
  free_arena(abp);
  mutex_.unlock();
}

void BitMap::set(size_t n) {
  mutex_.lock();
  Bitset::set(map(n));
  mutex_.unlock();
}

size_t BitMap::get(size_t n) {
  mutex_.lock();
  auto res = Bitset::get(map(n));
  mutex_.unlock();
  return res;
}

void BitMap::clear() {
  mutex_.lock();
  Bitset::clear();
  mutex_.unlock();
}

void BitMap::clear(size_t from, size_t to) {
  mutex_.lock();
  Bitset::clear(map(from), map(to));
  mutex_.unlock();
}
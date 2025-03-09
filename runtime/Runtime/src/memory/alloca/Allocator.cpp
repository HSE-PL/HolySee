#include "Allocator.hpp"


std::vector<size_t> partion_of_pages(size_t count_pages) {
  int count;
  for (count = 1; count * (1 << count) <= count_pages; ++count)
    ;

  --count;

  std::vector<size_t> counts;

  for (int i = count; i; counts.push_back(1 << i--))
    ;

  size_t lost_pages = count_pages - count * (1 << count);

  auto lost_partion = lost_pages < 8 ? std::vector<size_t>(lost_pages, 1)
                                     : partion_of_pages(lost_pages);

  for (int i = 0; i < lost_partion.size(); ++i)
    counts[i] += lost_partion[i];

  return counts;
}

Allocator::Allocator(size_t start_heap, size_t size_heap)
    : Heap(), start(start_heap), size(size_heap),
      regions(std::vector<Region<Arena>>()) {
  printf("heap on\n%zx\n%zx\n", start_heap, start_heap + size_heap);
  /*
   * variant of regions "double-double"
   * each egion is +- equal in size
   */
  size_t count_pages = size >> 12;

  auto counts = partion_of_pages(count_pages);

  size_t cur    = start_heap;
  size_t a_size = 1 << 12;
  size_t i      = 0;
  for (const size_t c : counts) {
    regions.emplace_back(cur, c, a_size, i);

    add_active(i++);

    cur += c * a_size;
    a_size <<= 1;
  }
}

size_t Allocator::alloc(size_t object_size) {
  auto arena = get_min_more_then(object_size);
  std::cout << "alloca " << object_size << " on arena " << arena << std::endl;
  size_t start_new_object = arena->cur;
  std::cout << start_new_object << std::endl;
  if (start_new_object == arena->start)
    add_active(arena->tier);

  std::cout << "try to del\n";
  del(arena);
  std::cout << "arena was del\n";
  arena->cur += object_size;
  append(arena);
  std::cout << "arena was replace\n";
  return start_new_object;
}

void Allocator::add_active(size_t index) {
  std::cout << "call add active\n";
  auto new_active = regions[index].pull.back();
  regions[index].pull.pop_back();

  // regions[index].count_empty++;

  std::cout << "call append new_active\n" << new_active << std::endl;
  print();
  append(new_active);
  std::cout << "return from add_active\n";
}

Arena* Allocator::arena_by_ptr(size_t ptr) const {
  int index;
  for (index = 0; index < regions.size() && ptr >= regions[index].start;
       ++index)
    ;
  --index;
  auto offset  = ptr - regions[index].start;
  auto a_index = offset / regions[index].t_size;
  return regions[index].items[a_index];
}

void Allocator::free_arena(Arena* a) {
  del(a);
  keys.erase(a);
  a->cur = a->start;
  regions[a->tier].pull.push_back(a);
}

void Allocator::free(size_t ptr) {
  std::cout << "free     " << ptr << std::endl;
  std::cout << "on arena " << arena_by_ptr(ptr)->start << std::endl;

  free_arena(arena_by_ptr(ptr));
}

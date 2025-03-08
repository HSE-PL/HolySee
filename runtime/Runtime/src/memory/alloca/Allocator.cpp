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

Allocator::Allocator(const size_t start_heap, const size_t size_heap)
    : Heap(), start(start_heap), size(size_heap),
      regions(std::vector<Region<Arena>>()) {
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
  std::cout << "alloca " << object_size << std::endl;
  size_t start_new_object = arena.cur;
  if (start_new_object == arena.start)
    add_active(arena.tier);
  del(arena);
  arena.cur += object_size;
  append(arena);
  return start_new_object;
}

void Allocator::add_active(size_t index) {
  auto new_active = regions[index].items.back();
  regions[index].items.pop_back();

  regions[index].count_empty++;
  append(new_active);
}
